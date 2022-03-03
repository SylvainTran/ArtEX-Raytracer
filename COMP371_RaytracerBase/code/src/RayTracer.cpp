// Standard
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <math.h>       /* tan, pow */
#include <time.h>       /* srand */
#include <algorithm>

// Time
#include <ctime>
#include <ratio>
#include <chrono>

// My code
#include "HitRecord.h"
#include "RayTracer.h"
#include "Triangle.h"
#include "Rectangle.h"
#include "JSONSceneParser.h"
#include "Utility.hpp"

// File streams
std::ofstream ofs;
std::ofstream speed_test_fs;

// Eigen
#include <Eigen/Geometry>

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
bool local_illum = false; // ---> Switch this to have local or global illum.
bool shadows = false;
bool antialiasing = false;
const float MIN_RAY_DISTANCE = 0.0f;
const float MAX_RAY_DISTANCE = INFINITY;
// GI OPTIONS
// ----------
const int NB_SAMPLES_PER_PIXEL = 10;
const int MAX_NB_BOUNCES = 1; // the number of bounces/ray depth before terminating a path
int current_path_samples = 0; // light path samples iterator
int current_path_bounces = 0; // light path bounces iterator
bool stratified_sampling = false;
const unsigned int NB_CELLS_X = 10;
const unsigned int NB_CELLS_Y = 10;
const unsigned int CELL_SIZE = NB_SAMPLES_PER_PIXEL / (NB_CELLS_X * NB_CELLS_Y);
float p_termination_threshold = 0.333f;
float p = 0.0f;
float cos_theta;
const Vector3f light_color(1,1,1);
const Vector3f one(1,1,1);
const Vector3f zero(0,0,0);
Vector3f Li(0,0,0); // Li
Vector3f Le(0,0,0); // Le

// ----------------------------
// Simple True Beginner RayTracer (STBRT)
//
// ----------------------------
RayTracer::RayTracer() {

};
RayTracer::RayTracer(nlohmann::json& j) {
  this->j = j;
};
RayTracer::~RayTracer() {

};
RayTracer& RayTracer::operator=(RayTracer& other) {
    // TODO: std::move() on other.members for this members'
    return *this;
};
bool RayTracer::validateSceneJSONData() {
  this->geometryRenderList = std::vector<Surface*>();
  this->lights = vector<Light*>();
  auto jsp = new JSONSceneParser(j);
    
  if(!jsp->test_parse_geometry() || !jsp->test_parse_lights() || !jsp->test_parse_output()) {
        cout<<"One of the tests failed. Aborting..."<<endl;
        return false;
    } else {
//        cout<<"parsing geometry!"<<endl;
//        cout<<"parsing output (camera, etc.)!"<<endl;
        jsp->parse_geometry(this);
        jsp->parse_lights(this);
        jsp->parse_output(this);
    }
    return true;
};
void RayTracer::addGeometry(Surface* s) {
    this->geometryRenderList.push_back(s);
//    s->info();
//    std::cout<<"Added geometry to render list!"<<std::endl;
//    std::cout<<"New list size: "<<this->geometryRenderList.size()<<std::endl;
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
            diffuseColor += (kd * dc * cos_theta).cwiseProduct(id).cwiseProduct(lightColor);
        }
    }
    //cout<<"diffuse color: "<<diffuseColor<<endl;
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
    Vector3f specularReflectance = (ks * sc * shininess).cwiseProduct(is).cwiseProduct(lightColor);
   
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
    Matrix3f matrix_R {
        {frame_left(0),frame_left(1),frame_left(2)},
        {intersected_point_normal(0),intersected_point_normal(1),intersected_point_normal(2)},
        {frame_z(0),frame_z(1),frame_z(2)}
    };
    random_dir_vector = intersected_point + (matrix_R.inverse() * rand_point);
};
// radiance at point x
//
// This calculation corresponds to the recursively defined Lf term in the Rendering equation
// -----------------------------------------------------------------------------------------
Vector3f radiance_accumulator(0,0,0);
Vector3f RayTracer::radiance(HitRecord& currentHit, Vector3f o, Vector3f d) {
    
    Vector3f hitPoint;
    
    // Check for bounces on all surfaces in the scene
    // ----------------------------------------------
    for(Surface* s : this->geometryRenderList) {
        
        // Compute a new ray in the path
        // Given previous origin and direction vectors
        // -------------------------------------------
        Ray ray(o, d); // from last point of intersect x, to a random dir d
        
        // New intersection test for a random surface
        // ------------------------------------------
        bool hitSomething = s->hit(ray, 0, 1000, currentHit);
        
        if(hitSomething) {
            // Rendering Equation Parameters
            // -----------------------------
            
            // Hit point intersected (x)
            // -----------------------
            hitPoint = ray.evaluate(currentHit.t);
            Vector3f random_dir_vec(0,0,0);
            
            // Random dir vec (w)
            // ------------------
            getRandomVector(ray, hitPoint.normalized(), currentHit.n, random_dir_vec);
            random_dir_vec = random_dir_vec.normalized();
            
            // Normal (n)
            // ----------
            Vector3f n = currentHit.n;
            
            // cos(theta)
            // -----------
            cos_theta = Utility::max(n.dot(d), 0.0f); // d = ki
            
            // 'Le' term update for the new hitPoint x
            // ---------------------------------------
            Le = getDiffuseReflection(currentHit, ray, hitPoint);
            // Le = getAmbientReflectance(s->mat->ac, s->mat->ka);

            // Apply Russian roulette (p)
            // --------------------------
            p = drand48();
                                    
            if(current_path_bounces > MAX_NB_BOUNCES || p < p_termination_threshold) {
                                
                // Shoot a ray last intersection point (i.e., current final path)
                // to all the light sources in scene (gather irradiance)
                // ---------------------------------------
                Li = zero;
                for(Light* l : lights) {
                    Li += l->illuminate(ray, currentHit);
                }
                
                // Reset global path bounces
                // -------------------------
                current_path_bounces = 0;
                return Li;
            } else {
                // This is the integration or the recursive radiance walk along random paths in the hemisphere
                // Uses cos(theta) for the random sampling cosine weight correction
                // ---------------------------------------------------------------------------------
                return Le + (cos_theta * radiance(currentHit, hitPoint, random_dir_vec));
            }
            
            // Update the light path
            // ---------------------
            ++current_path_bounces;
            
        } else {
            // No hit => background color
            // --------------------------
            return this->activeSceneCamera->bkc;
        }
    }
};
void RayTracer::partitionSpace(std::vector<float[]>& vertices, float screenWidth, float screenHeight, float near, float far) {
    
};
void RayTracer::modelViewTransformation(Eigen::Matrix4f& MVP, Surface& s) {
    // Model transform
    Eigen::Matrix4f transformation {
        {s.centre(0),0,0,0},
        {0,s.centre(1),0,0},
        {0,0,s.centre(2),0},
        {0,0,0,1}
    };
    MVP = transformation * MVP;
};
bool RayTracer::groupRaycastHit(Ray& ray, float t0, float t1, HitRecord& hitReturn) {
    
    // HIT RECORD SETUP
    // ----------------
    HitRecord* currentHit = new HitRecord(t1); // Start ray at max range
    HitRecord* closestHit = currentHit;
    HitRecord* shadowHit(currentHit);

    // Iteration parameters
    // --------------------
    float minT1 = t1; // The minimum closest hit to viewer
    bool hitSomethingAtAll = false; // Should return bkg color if no hit

    // AMBIENT LIGHT
    // -------------
    float ka;
    Vector3f ai, ac, dc;
    ai = this->ambientIntensity;
  
    for(Surface* s : this->geometryRenderList) {
        
        // GEOMETRY INTERSECTION TEST
        // --------------------------
        bool hitSomething = s->hit(ray, t0, t1, *currentHit);
        
        if(hitSomething) {
            // NO BKG COLOR TEST
            // -----------------
            hitSomethingAtAll = true;
            
            // CLOSEST HIT: MIN T TEST
            // -----------------------
            if(abs(currentHit->t) < abs(minT1)) {
                minT1 = currentHit->t;
                t1 = minT1;
                closestHit = currentHit;

                // LIGHT PARAMETERS
                // ----------------
                Vector3f light_sum(0,0,0); // THE LIGHT SUM
                Vector3f lightOutput; // THE FINAL OUTPUT
                
                // GLOBAL ILLUMINATION STUFF
                // -------------------------------
                ka = closestHit->m->mat->ka; // Ambient coefficient
                ac = closestHit->m->mat->ac; // Ambient color
                dc = closestHit->m->mat->dc; // Diffuse material color used for integration
                Vector3f aL = ka * ac.cwiseProduct(ai);

                // LOCAL ILLUMINATION : PRIMARY RAY
                // --------------------------------
                if(local_illum) {

                    // ACCUMULATE LIGHT SOURCES
                    // ------------------------
                    // bool pointIsInShadow = false;
                    for(Light* l : lights) {
                        if(l->type == "point") {
                            light_sum += l->illuminate(ray, *closestHit);
                        } else if(l->type == "area") {
                            // create points i,j,k over the surface area
                            // by sampling over unit square area and projecting it around the area light
                            
                        }
                        //cout<<"light sum: " <<light_sum<<endl;
                    }
                    light_sum += aL; // THE REFLECTION EQUATION
                    // CLAMP
                    // -----
                    lightOutput = Utility::clampVectorXf(light_sum, 0.0, 1.0);
                    // LOCAL ILLUMINATION OUTPUT COLOR
                    // DIFFUSE OR SPECULAR DEBUG
                    // -------------------------
                    closestHit->color = lightOutput;
                    //
                    // COLOR DEBUG
                    // -----------
                    // closestHit->color = (ray.d.normalized() + one)/2;
                    //
                    // "NORMALS" DEBUG
                    // ---------------
                    // closestHit->color = (light_sum + one)/2;
                } else {
                    // GLOBAL ILLUMINATION : PRIMARY + SECONDARY RAYS
                    // ----------------------------------------------
                    
                    // LIGHT PATH BUILDING PARAMETERS
                    // ------------------------------
                    srand((unsigned)time( NULL ));
                    
                    // Iteration parameters
                    // --------------------
                    Vector3f light_path(0,0,0);
                    Vector3f indirect_iillum(0,0,0);
                    Vector3f direct_illum(0,0,0);
                    Vector3f random_dir_vec(0,0,0);
                    Vector3f hitPoint = ray.evaluate(closestHit->t);
                    getRandomVector(ray, hitPoint.normalized(), closestHit->n, random_dir_vec);
                    random_dir_vec = random_dir_vec.normalized();
                    Vector3f n = currentHit->n;
                    cos_theta = Utility::max(n.dot(random_dir_vec), 0.0f); // d = ki
                    
                    // Start light path building
                    // Add the light path between the viewer and the first point of intersection
                    // -----------------------------
                    for(Light* l : lights) {
                        direct_illum += l->illuminate(ray, *closestHit);
                    }
                    // Direct illumination part
                    // ------------------------
                    Li = getDiffuseReflection(*closestHit, ray, hitPoint); // + getSpecularReflectance(*closestHit, ray, n);
                    direct_illum += (Li) * getLambertianBRDF(1);
                    direct_illum = Utility::clampVectorXf(direct_illum, 0.0f, 1.0f);
                    // cout<<"direct ill: " <<direct_illum<<endl;
                                                      
                    // Indirect illumination part
                    // --------------------------
                    if (stratified_sampling) {
                        for(int i = 0, j = 0; i < NB_CELLS_Y; i++) {
                            do {
                                indirect_iillum += radiance(*closestHit, hitPoint, random_dir_vec) * cos_theta;
                                // cout<<"indirect_iillum: "<<indirect_iillum<<endl;
//
//                                p = drand48();
//                                if(p < p_termination_threshold) {
//                                    break;
//                                }
                                j++;
                            } while (j < NB_CELLS_X);
                            // Divide by cell size to reduce variance
                            // --------------------------------------
                            indirect_iillum /= current_path_samples;
                            
                            // Put it together
                            // ---------------
                            light_path += indirect_iillum;
                            light_path = light_path * getLambertianBRDF(1);
                            
                            // Reset indirect illum sum and path sample count for next cell
                            // -------------------------------------
                            indirect_iillum = zero;
                            j = 0;
                        }
                    } else {
                        do {
                            // Start the radiance walk over the path
                            // -------------------------------------
                            indirect_iillum += radiance(*closestHit, hitPoint, random_dir_vec) * cos_theta;
                            // cout<<"Cos theta: "<<cos_theta<<endl;
                            // cout<<"indirect_iillum: "<<indirect_iillum<<endl;
                                                        
//                            p = drand48();
//                            if(p < p_termination_threshold) {
//                                break;
//                            }
                            // Update path sampling count
                            // --------------------------
                            ++current_path_samples;
                            
                        } while (current_path_samples < NB_SAMPLES_PER_PIXEL);
                        // Divide by N samples to take the average
                        // ---------------------------------------
                        // cout<<"light_path before division: "<<light_path<<endl;
                        indirect_iillum /= (current_path_samples);
                        light_path += indirect_iillum;
                    }
                    
                    // Putting everything together:
                    // ----------------------------
                    // cout<<"light_path after division and clamp: "<<light_path<<endl;
                    light_path += direct_illum;
                    light_path = light_path * getLambertianBRDF(1);
                    
                    // Gamma correction
                    // ----------------
                    light_path(0) = pow(light_path(0), 1.0f/2.2f);
                    light_path(1) = pow(light_path(1), 1.0f/2.2f);
                    light_path(2) = pow(light_path(2), 1.0f/2.2f);
                    
                    // Set the total light path radiance with closestHit's surface color and divide by lambertian BRDF (the 1/PI)
                    // -----------------------------------------------
                    closestHit->color = light_path;
                    //cout<<"N path samples used: "<<current_path_samples<<endl;
                    current_path_samples = 0;
                }
            } // Local vs. Global
            
            // SHADOW RAY (TODO: ...)
            // ----------
            //
//          for(Light* l : lights) {
//                        for(Surface* s2 : this->geometryRenderList) {
//                            Vector3f x = (ray.evaluate(currentHit->t));
//                            Vector3f light_ray = (l->centre - x);
//                            light_ray = light_ray.normalized();
//                            Vector3f offsetFromPointX(0.0f, 0.1f, 0.0f);
//                            Ray shadowRay(x + offsetFromPointX, light_ray);
//                            bool srec = s2->hit(shadowRay, 0.0f, 1000.0f, *shadowHit);
//
//                            if(!srec) {
//                                light_sum += l->illuminate(ray, *closestHit);
//                            } else {
//                                hitReturn.color = zero;
//                                break;
//                            }
//          }
            // ANTIALIASING RAYS (AA)
            // ----------------------
            if (antialiasing) {
              // ANTIALIASING OPTION
              // -------------------
              // Shoot more rays and take average
            }
        } // Closest hit to viewer
    }// Surface list loop
    
  // COLOR OUTPUT FOR THIS PIXEL
  // ---------------------------
  if(!hitSomethingAtAll) {
    // hitReturn.color = colorNoHit;
    // hitReturn.color = this->activeSceneCamera->bkc;
    // COLORS DEBUG
    // -------------
    // hitReturn.color = (ray.d.normalized() + one)/2;
    return false;
  } else {
    hitReturn.color = closestHit->color;
  }
  return true;
};
// Computes the size of a pixel according to horizontal and vertical fov
// 2*tan(fov/2)/width = 1 horizontal unit assuming square pixels (the ratio is already adjusted)
// ----------
float computePixelSizeDeltaHorizontal(float horizontal_fov, float height) {
  return 2*tan(Utility::degToRad(horizontal_fov/2))/height;
};
float computePixelSizeVertical(float vertical_fov, float height) {
  return 2*tan(Utility::degToRad(vertical_fov/2))/height;
};
int RayTracer::save_ppm(const std::vector<float>& buffer, int dimx, int dimy) {
    for (unsigned int j = 0; j < dimy; j++) {
      for (unsigned int i = 0; i < dimx; i++) {
        ofs << (char) (255.0 * buffer[3*j*dimx+3*i + 0]) <<  (char) (255.0 * buffer[3*j*dimx+3*i + 1]) << (char) (255.0 * buffer[3*j*dimx+3*i+2]);
      }
    }
    cout<<endl;
    cout<<"Three centuries later..."<<endl;
    ofs.close();
    return 0;
};
void RayTracer::printUsefulLogs() {
    cout << "Running raytracer on scene: "<< this->activeSceneCamera->filename << endl;
    cout << "\nLocal illumination (if 0 => global): " << local_illum << endl;
    cout << "\nShadows (true = 1): " << shadows << "\nAntialiasing: " << antialiasing << endl;
    if(!local_illum) {
        cout<< "\nSamples per pixel: " << NB_SAMPLES_PER_PIXEL << "\nMax bounces: " << MAX_NB_BOUNCES << "\nP termination threshold: " << p_termination_threshold << "\nstratified sampling: " << stratified_sampling << "\nCELL_SIZE: " << NB_CELLS_Y << endl;
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
void RayTracer::logSpeedTest(duration<double> time_span) {
    cout << endl;
    cout << "Speed Test Results:" << endl;
    cout << "\"The RayTracer\": It took me " << time_span.count() << " seconds to run the scene." << endl;
    cout << "\"The RayTracer\": Your pretty image was outputted to /build/"<<this->activeSceneCamera->filename<<".\nThe speed test results you love so much were outputted to /build/speed_test.txt. Program terminated."<<endl;
    cout << endl;
    // Write to file speed test
    // ------------------------
    speed_test_fs.open("speed_test.txt", std::ios_base::out | std::ios_base::binary | std::ofstream::app);
    speed_test_fs << "\ntest file name: " << this->activeSceneCamera->filename << "\nlocal illumination (if 0 => global): " << local_illum << "\nN geometry: " << this->geometryRenderList.size() << "\nN lights: " << this->lights.size() << endl;
    speed_test_fs << "\nShadows (true = 1): " << shadows << "\nAntialiasing: " << antialiasing << endl;
    if(local_illum) {

    } else {
        speed_test_fs << "\nSamples per pixel: " << NB_SAMPLES_PER_PIXEL << "\nMax bounces / ray depth: " << MAX_NB_BOUNCES << "\nP termination threshold: " << p_termination_threshold << "\nstratified sampling: " << stratified_sampling << "\nCELL_SIZE: " << NB_CELLS_Y << endl;
    }
    speed_test_fs << endl << "\nresult (s): " << time_span.count() << endl << endl << "test end\n" << "------";
    speed_test_fs.close();
}
void RayTracer::run() {
    // Validate scene data
    // -------------------
    if (!this->validateSceneJSONData()) {
      cout<<"Invalid scene! Aborting..."<<endl;
      return;
    }
    // Other guard checks
    // ------------------
    if (this->geometryRenderList.size() == 0) {
        cout<<"no geometry to render!"<<endl;
        return;
    }
    // Useless and Useful Logs
    // -----------------------
    printUselessLogs(0);
    printUsefulLogs();
    
    // Start timer
    // -----------
    steady_clock::time_point t1 = steady_clock::now();

    // Camera setup: Build the view camera's axes and setup its parameters
    // -------------------------------------------------------------------
    float dimx = this->activeSceneCamera->size(0);
    float dimy = this->activeSceneCamera->size(1);
    float fov = this->activeSceneCamera->fov;
    float aspect_ratio = dimx/dimy;
    float horizontal_fov = fov;
    float vertical_fov = Utility::radToDeg(2*atan2(tan(Utility::degToRad(horizontal_fov/2)), aspect_ratio));
    float scale_v = tan(Utility::degToRad(vertical_fov/2));
    float scale_h = tan(Utility::degToRad(horizontal_fov/2));

    Vector3f cam_position = this->activeSceneCamera->centre;
    Vector3f cam_forward = this->activeSceneCamera->lookat; // already neg: -z
    Vector3f cam_up = this->activeSceneCamera->up;
    Vector3f cam_left = cam_forward.cross(cam_up);
    //Vector3f cam_left = -1*(cam_forward.cross(cam_up));
    cam_up = cam_forward.cross(cam_left); // Actual up
    cam_up *= -1; // flip it up
        
    // Raycasting setup
    // ----------------
    float delta = computePixelSizeDeltaHorizontal(horizontal_fov, dimy); // The direct pixel size
    //float delta2 = computePixelSizeVertical(vertical_fov, dimy);
    Vector3f A = cam_position + (1)*cam_forward;
    Vector3f BA = A + (cam_up * (delta*(dimy/2)));
    Vector3f CB = BA - (cam_left * (delta*(dimx/2)));
    
    // Output buffer and output ppm file open
    // --------------------------------------
    std::vector<float> buffer = std::vector<float>(3*dimx*dimy);
    ofs.open("test_result.ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6" << endl << dimx << ' ' << dimy << endl << "255" << endl;

    // Raycasting other parameters
    // ---------------------------
    Ray currentRay; // the current ray to raycast
    Vector3f ray_nds; // the raycasted ray in nds
    HitRecord* rayColor = new HitRecord(MAX_RAY_DISTANCE); // the ray color to use
    bool hitSomething = false; // if the current ray hit a geometry in the scene
    
    // Rendering loop
    // --------------
    for(int j = dimy-1; j >= 0; j--) {
      for(int i = 0; i < dimx; i++) {
          
        // Simulate projection through sweeping
        // ------------------------------------
        ray_nds = CB + (i*delta+delta/2)*cam_left - (j*delta+delta/2)*cam_up - cam_position; // Adjusts cam position
        currentRay.setRay(cam_position, ray_nds);
        
        // Intersection Test
        // -----------------
        // cout<<"ray nds: "<<ray_nds<<endl;
        hitSomething = groupRaycastHit(currentRay, MIN_RAY_DISTANCE, MAX_RAY_DISTANCE, *rayColor);
        if(!hitSomething) {
            continue;
        }
        // COLOR DEBUG 1
        // -------------
        // Vector3f tmp = currentRay.d.normalized();
          
        // Buffer out
        // ----------
        buffer[3*j*dimx+3*i+0] = rayColor->color(0);
        buffer[3*j*dimx+3*i+1] = rayColor->color(1);
        buffer[3*j*dimx+3*i+2] = rayColor->color(2);
        
        // COLOR DEBUG 2
        // -------------
        // buffer[3*j*dimx+3*i+0] = (tmp(0) + 1)/2;
        // buffer[3*j*dimx+3*i+1] = (tmp(1) + 1)/2;
        // buffer[3*j*dimx+3*i+2] = (tmp(2) + 1)/2;
      }
    }
    // Write to ppm
    // ------------
    save_ppm(buffer, dimx, dimy);
    
    // Stop Speed Timer
    // ----------------
    steady_clock::time_point t2 = steady_clock::now();
    auto time_span = duration_cast<duration<double>>(t2 - t1);
    
    // Write to file speed test
    // ------------------------
    logSpeedTest(time_span);
    
    // Clean-up
    // --------
    delete rayColor;
};
