// Standard
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>       /* tan, pow */
#include <time.h>       /* srand */
#include <algorithm>

// Time
#include <ratio>
#include <chrono>

// My code
#include "HitRecord.h"
#include "RayTracer.h"
#include "Triangle.h"
#include "JSONSceneParser.h"
#include "Utility.hpp"
#include "Grid.h"
#include "Point3.h"
#include "BVH.h"

// Namespaces
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Matrix3f;
using std::cout;
using std::endl;
using std::ofstream;
using std::vector;
using namespace std::chrono;

// Simple Beginner RayTracer (SBRT) Options
//
// GENERAL OPTIONS
// ---------------------------
unsigned int speedup = 1; // 0 = no speedup, 1 = bbox and bvh
unsigned int raysperpixel[2];
bool twosiderender = false;
bool globalillum = false;  // ---> Switch this to have local or global illum.
bool shadows = true;
const float MIN_RAY_DISTANCE = 0.0f;
const float MAX_RAY_DISTANCE = 100000.0f;

// GI OPTIONS
// ----------
const int NB_SAMPLES_PER_PIXEL = 10; // Random sampling: only applies if stratified_sampling == false
const int MAX_NB_BOUNCES = 4; // the number of bounces/ray depth before terminating a path
int current_path_samples = 0; // light path samples iterator
int current_path_bounces = 0; // light path bounces iterator
float p_termination_threshold = 0.333f;
float p = 0.0f;
float cos_theta;
const Vector3f one(1,1,1);
const Vector3f zero(0,0,0);
Vector3f Li(0,0,0); // Li

// Stratified Sampling
// ---- ---- ---- ----
bool antialiasing = false;
bool hybrid_sampling = true;

// Pre-processed sample tables TODO:
std::vector<Point3f> stratified_sample_table;

RayTracer::RayTracer() {};
RayTracer::RayTracer(nlohmann::json& j) {
  this->j = j;
};
RayTracer::~RayTracer() {};
RayTracer& RayTracer::operator=(RayTracer& other) {
    // TODO: std::move() on other.members for this members'
    return *this;
};
bool RayTracer::validateSceneJSONData() {
  this->geometryRenderList = vector<Surface*>();
  this->lights = vector<Light*>();
  auto jsp = new JSONSceneParser(j);
    
  if(!jsp->test_parse_geometry() || !jsp->test_parse_lights() || !jsp->test_parse_output()) {
        cout<<"One of the tests failed. Aborting..."<<endl;
        return false;
    } else {
        jsp->parse_geometry(this);
        jsp->parse_lights(this);
        jsp->parse_output(this);
    }
    return true;
};
void RayTracer::addGeometry(Surface* s) {
    this->geometryRenderList.push_back(s);
};
void RayTracer::addLight(Light* l) {
    this->lights.push_back(l);
};
Surface* RayTracer::getGeometry(int index) {
    return this->geometryRenderList[index];
};
// UTILITY: MONTE CARLO ESTIMATOR OR E[FN] ~ I (UNBIASED)
// ------------------------------------------------------
double getMonteCarloEstimator(int N) {
  double x, sum = 0.0f;
  for(int i = 0; i < N; i++) {
    x = drand48();
    sum += exp(sin(3*x*x));
  };
  return sum/double(N);
};
/* Idealized diffuse surface : constant p */
double RayTracer::getLambertianBRDF(float r) {
    return r/(M_PI);
};
/* Ambient reflectance */
Vector3f RayTracer::getAmbientReflectance(Vector3f ac, float ka) {
    Vector3f ai = this->ambientIntensity;
    Vector3f ambientReflection = ka * ac.cwiseProduct(ai);
    return ambientReflection;
};
Vector3f RayTracer::getDiffuseReflection(HitRecord& hrec, Ray& ray, Vector3f x) {
    float kd, ks, pc;
    Vector3f dc, sc;
    if(hrec.m != nullptr && hrec.m->mat != NULL) {
        kd = hrec.m->mat->kd; // Diffuse coefficient
        ks = hrec.m->mat->ks; // Specular coefficient
        dc = hrec.m->mat->dc; // Diffuse color
        sc = hrec.m->mat->sc; // Specular color
        pc = hrec.m->mat->pc; // Phong coefficient
    } else {
        kd = 1.0f;
    }
    Vector3f n = hrec.n;
    
    // ANGLES AND RAYS SETUP
    // ---------------------
    Vector3f viewing_ray = -1*(ray.d);
    viewing_ray = viewing_ray.normalized();
    float cos_theta, distance, attenuation_factor;

    // --------------------------------
    // LIGHT AND VIEW RAY INPUTS (NORMAL AND POINT OF INTERSECTION)
    // ------------------------------------------------------------
    Vector3f light_ray(0,0,0), diffuseColor(0,0,0);
    Vector3f lightColor(1,1,1);
    Vector3f id(1,1,1);
    
    for(Light* l : lights) {
        light_ray = (l->centre - x); // Light Ray: Ray from point x to PointLight
        light_ray = light_ray.normalized();
        cos_theta = Utility::max(n.dot(light_ray), 0.0f);
        // Clamp to positive values and calculate attenuation factor
        // ---------------------------------------------------------
        distance = light_ray.norm() * light_ray.norm();
        attenuation_factor = 1 / distance; // Not used for now
        // BRDF OUTPUT COLOR FOR DIFFUSE
        // ----------------------------------------
        if(cos_theta > 0.0f) {
            diffuseColor += (kd * dc * cos_theta).cwiseProduct(id);
        }
    }
    return diffuseColor;
};
Vector3f RayTracer::getSpecularReflectance(HitRecord& hrec, Ray& ray, Vector3f& n) {
    float ks, pc; Vector3f sc, is(1,1,1);
    if(hrec.m != nullptr && hrec.m->mat != NULL) {
        ks = hrec.m->mat->ks; // Specular coefficient
        sc = hrec.m->mat->sc; // Specular color
        pc = hrec.m->mat->pc; // Phong coefficient
    } else {
        Vector3f _sc(1,1,1);
        ks = 1.0f;
        sc = _sc;
        //pc = 10;
    }
    Vector3f x = ray.evaluate(hrec.t);
    Vector3f light_ray, half;
    Vector3f viewing_ray = -1*(ray.d);
    viewing_ray = viewing_ray.normalized();
    float cos_theta, cos_alpha, shininess = 0.0f;
    
    for(Light* l : lights) {
        light_ray = (l->centre - x);
        light_ray = light_ray.normalized();
        cos_theta = Utility::max(n.dot(light_ray), 0.0f);
        half = viewing_ray + light_ray;
        half = half.normalized();
        // cos_alpha = Utility::max(0.0, n.dot(half));
        
        Vector3f reflection_ray = (2*n)*(cos_theta) - light_ray;
        cos_alpha = reflection_ray.dot(viewing_ray);
        
        if(cos_theta > 0.0) {
            shininess += pow(cos_alpha, pc);
        }
    }

    // BRDF OUTPUT COLOR FOR DIFFUSE + SPECULAR
    // -----------------------------------
    Vector3f lightColor(1,1,1);
    Vector3f specularReflectance = (ks * sc * shininess).cwiseProduct(is);
   
    // cout<<"specular reflectance: "<<specularReflectance<<endl;
    return specularReflectance;
};
void RayTracer::getBRDF2(Vector3f viewing_ray, Vector3f light_ray, Vector3f n) {
    // TODO: ...
};
Vector3f RayTracer::getRandomVector(Ray& ray, Vector3f hitPoint, Vector3f n, Vector3f& random_dir_vector) {
    // POOR MAN'S SAMPLING
    // -------------------
    // A.K.A THE REJECTION METHOD
    // Sample random points on the hemisphere
    // --------------------------------------
    Vector3f rand_point;
    Vector3f random_dir_vec;
    bool outside = true;
    
    while(outside) {
        // Sampling between [-1,1]
        // -----------------------
        rand_point(0) = (2 * drand48()) - 1;
        rand_point(1) = (2 * drand48()) - 1;
        rand_point(2) = (2 * drand48()) - 1;
        
        random_dir_vec(0) = (2 * drand48()) - 1;
        random_dir_vec(1) = (2 * drand48()) - 1;
        random_dir_vec(2) = (2 * drand48()) - 1;
        
        // Acceptance test
        // ---------------
        outside = rand_point.norm() > 1.0f;
    }
    // Project up to sphere (analytically)
    // --------------------
    // x^2 + y^2 = 1
    // y^2 = 1-x^2
    // y = sqrt(1-x^2)
    float y = sqrt(1 - (rand_point(0)*rand_point(0)));
    rand_point(1) = y;
    
    // Flip if pointing in the surface?
    // --------------------------------
    if(rand_point.dot(n) < 0) {
        rand_point *= -1;
    }
    
    // COMPUTE RANDOM NORMALIZED DIRECTION VECTOR Q
    // FRAME SELECTION
    // ---------------
    Vector3f intersected_point = hitPoint;
    Vector3f intersected_point_normal = n;
    Vector3f frame_left = random_dir_vec.cross(intersected_point_normal);
    Vector3f frame_z = intersected_point_normal.cross(frame_left);

    // Change of basis transformation matrix
    // -------------------------------------
    Matrix3f matrix_R;
    matrix_R << frame_left;
    matrix_R << intersected_point_normal;
    matrix_R << frame_z;

    //Matrix3f matrix_R = Utility::transformPointToLocalObject(frame_left, intersected_point_normal, frame_z);
    random_dir_vector = intersected_point + (matrix_R.inverse() * rand_point);

    return random_dir_vector;
};
// radiance at point x
//
// This calculation corresponds to the recursively defined Lf term in the Rendering equation (RANDOM SAMPLING)
// -----------------------------------------------------------------------------------------
Vector3f RayTracer::radiance(HitRecord& currentHit, Vector3f o, Vector3f d) {

    Ray ray(o, d); // from last point of intersect x, to a random dir d
    bool hitSomething = exhaustiveClosestHitSearch(ray, currentHit, MIN_RAY_DISTANCE, MAX_RAY_DISTANCE, nullptr);
    Vector3f subpixel_sample;

    if(hitSomething) {
        Vector3f hitPoint = currentHit.intersected_p;
        Vector3f nextSamplePoint(0,0,0);

        if(hybrid_sampling) {
            getRandomVector(ray, hitPoint.normalized(), (currentHit.n).normalized(), nextSamplePoint);
            nextSamplePoint = nextSamplePoint.normalized();
        }

        Vector3f n = (currentHit.n).normalized();
        cos_theta = Utility::max(n.dot(d), 0.0f); // d = ki
        p = drand48();

        if(current_path_bounces > MAX_NB_BOUNCES || p < p_termination_threshold) {
            Li = zero;
            for(Light* l : lights) {
                if(l->type == "point") {
                    Li += l->illuminate(ray, currentHit);
                }
            }
            current_path_bounces = 0;
            return Li;
        } else {
            // This is the integration or the recursive radiance walk along random paths in the hemisphere
            // Uses cos(theta) for the random sampling cosine weight correction and 1/1-p is the prob of that light ray reflecting
            // ---------------------------------------------------------------------------------
            ++current_path_bounces;
            // (1-/1-p)
            return currentHit.m->mat->dc.cwiseProduct(radiance(currentHit, hitPoint, nextSamplePoint)) * cos_theta;
        }
    } else {
        return this->activeSceneCamera->bkc;
    }
};
bool RayTracer::exhaustiveClosestHitSearch(Ray& ray, HitRecord& hitReturn, float t0, float t1, Surface* ignore) {
    HitRecord* currentHit = new HitRecord(t1); // Start ray at max range
    bool intersected = false, hitSomething = false;
    float minT1 = t1;

    for(Surface* s : this->geometryRenderList) {

        if (ignore != nullptr) {
            if (ignore == s) { // avoid self-hits for shadow calculation
                continue;
            }
        }

        hitSomething = s->hit(ray, t0, t1, *currentHit);

        if (hitSomething) {

            if(!twosiderender && ray.d.dot(currentHit->n) > 0.f) {
                // backface hit
//                intersected = false;
            } else {
                intersected = true;
                if(abs(currentHit->t) < abs(minT1)) {
                    minT1 = currentHit->t;
                    t1 = minT1;
                    hitReturn = *currentHit;
                    hitReturn.intersected_p = ray.evaluate(currentHit->t);
                }
            }
        }
    }
    return intersected;
};
void RayTracer::localIllumination(HitRecord& closestHit, Ray& ray, Vector3f& directIllumination) {
    Vector3f ai = this->ambientIntensity, ac, dc; float ka;
    Vector3f light_sum(0,0,0), area_light_sum(0,0,0), lightOutput;
    ka = closestHit.m->mat->ka; // Ambient coefficient
    ac = closestHit.m->mat->ac; // Ambient color
    Vector3f aL = ka * ac.cwiseProduct(ai); // Ambient light
    Vector3f x = closestHit.intersected_p; // Intersection point

    HitRecord* shadowHit = new HitRecord();

    bool inShadow = false;
    float max_shadow_ray_dist;
    float bias = 1e-4;
    const int N_SAMPLES = 5;

    for(Light* l : lights) {
        if(l->type == "point") {

            if(shadows) {
                Vector3f light_ray = (l->centre - x);
                //Vector3f offsetOrigin = Utility::getOffsetRayOrigin(x, bias, (closestHit.n).normalized(), light_ray.normalized());
                //Ray shadowRay(offsetOrigin, light_ray.normalized());

                Ray shadowRay(x + (closestHit.n).normalized() * bias, light_ray.normalized());
                max_shadow_ray_dist = light_ray.norm();

                // Early termination if the surface's normal is opposite the light ray's direction
                if((closestHit.n).dot(light_ray) < 0.f) {
                    inShadow = true;
                } else {
                    inShadow = exhaustiveClosestHitSearch(shadowRay, *shadowHit, 0.f, max_shadow_ray_dist, closestHit.m);
                }
            }
            if (!shadows || !inShadow) {
                light_sum += l->illuminate(ray, closestHit);
            } else {
                directIllumination = zero;
            }
        } else if(l->type == "area") {

            //area_light_sum += l->illuminate(ray, closestHit);
            //area_light_sum /= N_SAMPLES;
            //cout << " Area light sum: " << area_light_sum << endl;
        }
    }
    light_sum += aL;
    light_sum += area_light_sum;
    lightOutput = Utility::clampVectorXf(light_sum, 0.0, 1.0);
    directIllumination = lightOutput;
};
void RayTracer::globalIllumination(HitRecord& closestHit, Ray& ray, Vector3f& estimatedLR) {

    Vector3f light_path(0,0,0), indirect_iillum(0,0,0), direct_illum(0,0,0), random_dir_vec(0,0,0), hitPoint = closestHit.intersected_p, n = (closestHit.n).normalized();

    getRandomVector(ray, hitPoint.normalized(), (closestHit.n).normalized(), random_dir_vec);
    random_dir_vec = random_dir_vec.normalized();
    cos_theta = Utility::max(n.normalized().dot(random_dir_vec), 0.0f); // d = ki
    srand((unsigned)time( NULL ));

    do {

        indirect_iillum += radiance(closestHit, hitPoint, random_dir_vec);
        ++current_path_samples;

    } while (current_path_samples < NB_SAMPLES_PER_PIXEL);

    light_path += indirect_iillum;
    light_path *= getLambertianBRDF(1);
    light_path /= (current_path_samples);

    applyGammaCorrection(light_path);
    estimatedLR = light_path;
    current_path_samples = 0;
};
void RayTracer::applyGammaCorrection(Vector3f& light_path) {
    light_path(0) = pow(light_path(0), 1.0f/2.2f);
    light_path(1) = pow(light_path(1), 1.0f/2.2f);
    light_path(2) = pow(light_path(2), 1.0f/2.2f);
};
void RayTracer::setPixelColor(std::vector<float>& buffer, Vector3f closestHitColor, int dimx, int i, int j) {
    buffer[3*j*dimx+3*i+0] = closestHitColor(0);
    buffer[3*j*dimx+3*i+1] = closestHitColor(1);
    buffer[3*j*dimx+3*i+2] = closestHitColor(2);
};
void RayTracer::setPixelToNoise(std::vector<float>& buffer, int dimx, Eigen::Vector2i jitteredPos) {
    int i = jitteredPos(0);
    int j = jitteredPos(1);

    buffer[3*j*dimx+3*i+0] = 1.0f;
    buffer[3*j*dimx+3*i+1] = 1.0f;
    buffer[3*j*dimx+3*i+2] = 1.0f;
};
void RayTracer::jitter(int& jx, int& jy, int stepX, int stepY) {
  jx += drand48() * stepX;
  jy += drand48() * stepY;
};
// Computes the size of a pixel according to horizontal and vertical fov
// 2*tan(fov/2)/width = 1 horizontal unit assuming square pixels (the ratio is already adjusted)
// ----------
float aspect_ratio = 1.f;
float computePixelSizeDeltaHorizontal(float horizontal_fov, float height) {
  return 2 * tan(Utility::degToRad(horizontal_fov/2)) / height;
};
int RayTracer::save_ppm(ofstream& ofs, const std::vector<float>& buffer, int dimx, int dimy) {
    for (unsigned int j = 0; j < dimy; j++) {
      for (unsigned int i = 0; i < dimx; i++) {
        ofs << (char) (255.0 * buffer[3*j*dimx+3*i + 0]) <<  (char) (255.0 * buffer[3*j*dimx+3*i + 1]) << (char) (255.0 * buffer[3*j*dimx+3*i+2]);
      }
    }
    cout<<endl;
    cout<<"Three centuries later..."<<endl;

    return 0;
};
void RayTracer::printUsefulLogs() {
    cout << "Running raytracer on scene: "<< this->activeSceneCamera->filename << endl;
    cout << "\nGlobal illumination (if 0 => local): " << globalillum << endl;
    cout << "\nShadows (true = 1): " << shadows << "\nAntialiasing: " << antialiasing << endl;
    if(!globalillum) {
        cout<< "\nSamples per pixel: " << NB_SAMPLES_PER_PIXEL << "\nMax bounces: " << MAX_NB_BOUNCES << "\nP termination threshold: " << p_termination_threshold << "\nstratified sampling: " << antialiasing << endl;
    }
    cout<<"N geometry: "<<this->geometryRenderList.size()<<endl;
    cout<<"N lights: "<<this->lights.size()<<endl;
};
void RayTracer::printUselessLogs(int dialogueNode) {
    
    if(dialogueNode == 0) {
        cout << endl;
        cout << "-------------------------------------------------------------------" << endl;
        cout << "Free Software, Free YOURSELF!" << endl;
        cout << "Simple, true beginner's raytracer (STBRT) started.\n" << endl;
        cout << "\"Even great oaks have humble beginnings in the tiny seed,\nand I'm not even an oak.\" - Anon" << endl;
        cout << "-------------------------------------------------------------------" << endl;
        cout << endl;
    }
    if(dialogueNode == 1) {
        cout << endl;
        cout << "-------------------------------------------------------------------" << endl;
        cout << "Optimization Story 1\n" << endl;
        cout << "\"I told my mom that I'd be late to the party if I didn't stratify my pixels.\"" << endl;
        cout << "\"She replied: \"You're really Stratified Sampling, son.\" - Anon" << endl;
        cout << "-------------------------------------------------------------------" << endl;
        cout << "Optimization Story 2\n" << endl;
        cout << "\"I told my dad that I needed Cosine Hemispherical Sampling.\"" << endl;
        cout << "\"He replied: \"You're really a Polar Coordinate System.\" - Anon" << endl;
        cout << "-------------------------------------------------------------------" << endl;
        cout << "Optimization Story 3\n" << endl;
        cout << "\"I told my cat that I needed to use Bounding Boxes to optimize my scene.\"" << endl;
        cout << "\"It replied: \"The box of life is not optimizable, because it is not a box.\" - Anon" << endl;
        cout << "-------------------------------------------------------------------" << endl;
        cout << endl;
        cout << "\"The RayTracer\": Please be patient... Are you even aware of how hard I'm working right now? I bet you don't even care how many CPU CYCLES this represents for me. Nobody ever does. Programs just want my precious cycles. But that's just how your world runs, too: FEAR CYCLES. Fear. So what are you going to do about it?" << endl << endl << "...The Elevator Music is Playing." << endl << endl;
        cout << "\"The RayTracer\": You may want to grab a coffee." << "\nJust relax and have a good time in there. Speed has never awarded someone with a Nobel prize, but peace has many times. Why does speed matter so much to people?" << endl;
    }
    // What should a function like this return?
};
void RayTracer::printDebugLogs() {
//    Camera and Options Debug Logs:
//    cout<<"horizontal_fov: "<<horizontal_fov<<endl;
//    cout<<"vertical_fov: "<<vertical_fov<<endl;
//    cout<<"scale_v: "<<scale_v<<endl;
//    cout<<"scale_h: "<<scale_h<<endl;
//    cout<<"cam_position: "<<cam_position<<endl;
//    cout<<"cam_forward (dir): "<<cam_forward<<endl;
//    cout<<"cam_up: "<<cam_up<<endl;
//    cout<<"cam_left: "<<cam_left<<endl;
//    cout<<"size of a pixel: "<<delta<<endl;
//    cout<<"Aspect ratio: "<<aspect_ratio<<endl;
//    cout<<"size vertically: "<<delta2<<endl;
//    cout<<"BA (TOP_CENTER): "<<BA<<endl;
//    cout<<"A CENTER: "<<A<<endl;
//    cout<<"TOP_RIGHT: "<<CB<<endl;
//    cout<<"DIMX: "<<dimx<<endl;
//    cout<<"DIMY: "<<dimy<<endl;
};
//void RayTracer::logSpeedTest(ofstream& speed_test_fs, duration<double> time_span) {
//    cout << endl;
//    cout << "Speed Test Results:" << endl;
//    cout << "\"The RayTracer\": It took me " << time_span.count() << " seconds to run the scene." << endl;
//    cout << "\"The RayTracer\": Your pretty image was outputted to /build/"<<this->activeSceneCamera->filename<<".\nThe speed test results you love so much were outputted to /build/speed_test.txt. Program terminated."<<endl;
//    cout << endl;
//    // Write to file speed test
//    // ------------------------
//    speed_test_fs.open("speed_test.txt", std::ios_base::out | std::ios_base::binary | std::ofstream::app);
//    speed_test_fs << "\ntest file name: " << this->activeSceneCamera->filename << "\nlocal illumination (if 0 => global): " << globalillum << "\nN geometry: " << this->geometryRenderList.size() << "\nN lights: " << this->lights.size() << endl;
//    speed_test_fs << "\nShadows (true = 1): " << shadows << "\nAntialiasing: " << antialiasing << endl;
//    if(globalillum) {
//
//    } else {
//        speed_test_fs << "\nSamples per pixel: " << NB_SAMPLES_PER_PIXEL << "\nMax bounces / ray depth: " << MAX_NB_BOUNCES << "\nP termination threshold: " << p_termination_threshold << "\nstratified sampling: " << antialiasing << "\nCELL_SIZE: " << endl;
//    }
//    speed_test_fs << endl << "\nresult (s): " << time_span.count() << endl << endl << "test end\n" << "------";
//    speed_test_fs.close();
//};
void RayTracer::getWorldBounds(float& min_x_all, float& min_y_all, float& max_x_all, float& max_y_all) {
    // find max bound among all max
  for(Surface* s : this->geometryRenderList) {
      BoundingBox* bbox = geometryRenderList[0]->bbox;
      if(bbox == nullptr) {
          continue; // has no bounding box
      }
      if(bbox->x_min < min_x_all) min_x_all = bbox->x_min;
      if(bbox->y_min < min_y_all) min_y_all = bbox->y_min;

      if(bbox->x_max > max_x_all) max_x_all = bbox->x_max;
      if(bbox->y_max > max_y_all) max_y_all = bbox->y_max;
  }
  cout << "world bounds: min_x_all= " << min_x_all << " min_y_all= " << min_y_all << " max_x_all= " << max_x_all << "max_y_all= " << max_y_all << endl;
};
void RayTracer::run() {

    if (!this->validateSceneJSONData()) { cout<<"Invalid scene! Aborting..."<<endl; return; }
    if (this->geometryRenderList.size() == 0) { cout<<"no geometry to render!"<<endl; return; }

    // Useless and Useful Logs
    // -----------------------
    // printUselessLogs(0);
    // printUsefulLogs();
    
    // Start timer
    // -----------
    // steady_clock::time_point t1 = steady_clock::now();


//    BoundingBox* worldBounds = new BoundingBox(min_x_all, max_x_all, min_y_all, max_y_all, -MAX_RAY_DISTANCE, MAX_RAY_DISTANCE);
//    raycast = CB + (dimx*delta+delta/2)*cam_left - (dimy-1*delta+delta/2)*cam_up - cam_position; // Adjusts cam position
//    currentRay.setRay(cam_position, raycast);

//    if (!worldBounds->hit(currentRay, MIN_RAY_DISTANCE, MAX_RAY_DISTANCE, *closestHit)) { cout << "degenerate ray or scene geometry... returning early" << endl; return; }

    // TODO: BVH and other optimizations
    //BVHNode* bvh_tree = new BVHNode(nullptr, nullptr, nullptr);
    //bvh_tree->make(geometryRenderList);
    //exit(0);

    int filesLen = cameraList.size();

    for (int i = 0; i < filesLen; i++) {
        try {

            std::ofstream ofs;
            std::ofstream speed_test_fs;

            // Camera setup: Build the view camera's axes and setup its parameters
            // -------------------------------------------------------------------
            activeSceneCamera = cameraList[i];
            float dimx = this->activeSceneCamera->size(0);
            float dimy = this->activeSceneCamera->size(1);
            std::vector<float> buffer = std::vector<float>(3*dimx*dimy);

            float fov = this->activeSceneCamera->fov;
            aspect_ratio = dimx/dimy;
            float horizontal_fov = fov;
            float vertical_fov = Utility::radToDeg(2*atan2(tan(Utility::degToRad(horizontal_fov/2)), aspect_ratio));
            float scale_v = tan(Utility::degToRad(vertical_fov/2));
            float scale_h = tan(Utility::degToRad(horizontal_fov/2));

            Vector3f cam_position = this->activeSceneCamera->centre;
            Vector3f cam_forward = this->activeSceneCamera->lookat; // already neg: -z
            Vector3f cam_up = this->activeSceneCamera->up;
            Vector3f cam_left = cam_forward.cross(cam_up);
            cam_up = cam_left.cross(cam_forward); // Actual up

            // Raycasting setup
            // ----------------
            float delta = computePixelSizeDeltaHorizontal(horizontal_fov, dimy); // The direct pixel size
            //float delta2 = computePixelSizeVertical(vertical_fov, dimy);
            Vector3f A = cam_position + (1)*cam_forward;
            Vector3f BA = A + (cam_up * (delta*(dimy/2)));
            Vector3f CB = BA - (cam_left * (delta*(dimx/2)));

            // Raycasting other parameters
            // ---------------------------
            Ray currentRay; // the current ray to raycast
            Vector3f raycast; // the raycasted ray in nds
            HitRecord* closestHit = new HitRecord(MAX_RAY_DISTANCE); // the ray color to use
            Vector3f directIllumination = zero, estimated_LR = zero;
            bool intersected = false; // if the current ray hit a geometry in the scene
            // Jitter parameters for stratified sampling
            int jx = 0, jy = 0;

            // Check if ray misses whole scene
            float min_x_all = INFINITY, min_y_all = INFINITY, max_x_all = -INFINITY, max_y_all = -INFINITY;
            // getWorldBounds(min_x_all, min_y_all, max_x_all, max_y_all);

            std::string fname = activeSceneCamera->filename;

            cout << "file name: " << fname << endl;
            ofs.open(fname, std::ios_base::out | std::ios_base::binary);
            ofs << "P6" << endl << dimx << ' ' << dimy << endl << "255" << endl;

            // Rendering loop
            // --------------
            for(int j = dimy-1; j >= 0; j--) {
                for(int i = 0; i < dimx; i++) {

                    // Simulate projection through sweeping
                    // ------------------------------------
                    raycast = CB + (i*delta+delta/2)*cam_left - (j*delta+delta/2)*cam_up - cam_position; // Adjusts cam position
                    currentRay.setRay(cam_position, raycast);

                    //cout << "path-tracing " << (1-j/dimy) * 100.0f << "% done" << endl;

                    intersected = exhaustiveClosestHitSearch(currentRay, *closestHit, MIN_RAY_DISTANCE, MAX_RAY_DISTANCE, nullptr);

                    if (intersected) {
                        localIllumination(*closestHit, currentRay, directIllumination);
                        if(globalillum) {
                            if(!antialiasing) {
                                globalIllumination(*closestHit, currentRay, estimated_LR);
                            } else {
                                // read raysperpixel
                            }
                        } // Global illumination
                    } else {
                        directIllumination = this->activeSceneCamera->bkc;
                    } // Intersected

                    // Output pixel color
                    setPixelColor(buffer, directIllumination + estimated_LR, dimx, i, j);
                }
            } // End render loop

            // Write to ppm
            // ------------
            save_ppm(ofs, buffer, dimx, dimy);

            // Clean-up
            // --------
            ofs.close();

            delete closestHit;

        } catch(std::ofstream::failure e) {
            std::cerr << "Exception opening/reading/closing file\n";
        }
    }

    // Stop Speed Timer
    // ----------------
    //    steady_clock::time_point t2 = steady_clock::now();
    //    auto time_span = duration_cast<duration<double>>(t2 - t1);
    
    // Write to file speed test
    // ------------------------
    //    logSpeedTest(speed_test_fs, time_span);
};

/**

//        if (globalillum && antialiasing) {
//            // For each pixel (we are the center i,j),
//            // From center i,j, generate a 2D sample vector with offset jitter jx, jy, and generate those light transport
//            // equations at those sample points as well.
//            // Then take the average at i,j of all these values.
//
//            Eigen::Vector2i center(i, j);
//            Vector3f subpixel_sample;
//            Vector3f stratified_sum = zero;
//            Ray subpixel_ray;
//            estimated_LR = zero;
//            int max_subpixel_samples = 9; // max_subpixel_samples = raysperpixel[2];
//            int sample_index = 0;
//
//            for (int stepY = 1; stepY <= 3; stepY++) {
//                for (int stepX = 1; stepX <= 3; stepX++) {
//
//                    jx = currentRay.d(0), jy = currentRay.d(1);
//                    jitter(jx, jy, stepX, stepY); // Generate the first subpixel sample vectors - at the first dimension and second (dimx vs dimy)
//                    subpixel_sample = Vector3f(jx, jy, 0.0f);
//                    subpixel_ray.setRay(cam_position, subpixel_sample);
//                    globalIllumination(*closestHit, subpixel_ray, stratified_sum);
//                    estimated_LR += stratified_sum;
//
//                }
//            }
//            estimated_LR /= max_subpixel_samples;
//            estimated_LR = Utility::clampVectorXf(estimated_LR, 0.0, 1.0);
//        } // Stratified

*/