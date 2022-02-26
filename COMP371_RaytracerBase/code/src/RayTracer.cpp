// Standard
#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <math.h>       /* tan */

// External
#include <Eigen/Geometry>

// My code
#include "HitRecord.h"
#include "RayTracer.h"
#include "Triangle.h"
#include "Rectangle.h"
#include "JSONSceneParser.h"

std::ofstream ofs;
using std::cout;
using std::endl;
using std::ofstream;
using std::vector;
using Eigen::Vector3f;
using Eigen::Vector4f;

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
float degToRad(float deg) {
    return deg * (M_PI/180);
};
float radToDeg(float rad) {
    return rad * (180/M_PI);
};
bool RayTracer::validateSceneJSONData() {
  this->geometryRenderList = std::vector<Surface*>();
  this->lights = vector<Light*>();
  auto jsp = new JSONSceneParser(j);
  if(!jsp->test_parse_geometry() || !jsp->test_parse_lights() || !jsp->test_parse_output()) {
        cout<<"One of the tests failed. Aborting..."<<endl;
        return false;
    } else {
        cout<<"parsing geometry!"<<endl;
        cout<<"parsing output (camera, etc.)!"<<endl;
        jsp->parse_geometry(this);
        jsp->parse_lights(this);
        jsp->parse_output(this);
    }
    return true;
};
void RayTracer::addGeometry(Surface* s) {
    this->geometryRenderList.push_back(s);
    s->info();
    std::cout<<"Added geometry to render list!"<<std::endl;
    std::cout<<"New list size: "<<this->geometryRenderList.size()<<std::endl;
};
void RayTracer::addLight(Light* l) {
    this->lights.push_back(l);
};
Surface* RayTracer::getGeometry(int index) {
    return this->geometryRenderList[index];
};
Eigen::Vector3f RayTracer::clampVectorXf(Eigen::Vector3f value, float min, float max) {
  Eigen::Vector3f clamped;
  if(value(0) < min) {
    clamped(0) = min;
  } else if(value(0) > max) {
    clamped(0) = max;
  } else {
    clamped(0) = value(0);
  }

  if(value(1) < min) {
    clamped(1) = min;
  }
  if(value(1) > max) {
    clamped(1) = max;
  } else {
    clamped(1) = value(1);
  }

  if(value(2) < min) {
    clamped(2) = min;
  }
  if(value(2) > max) {
    clamped(2) = max;
  } else {
    clamped(2) = value(2);
  }
  return clamped;
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
bool RayTracer::groupRaycastHit(Ray& ray, float t0, float t1, HitRecord& hitReturn) {
    
    // HIT RECORD SETUP
    // -------------
    HitRecord* currentHit = new HitRecord(t1); // Start ray at max range
    HitRecord* closestHit = currentHit;
    Vector3f colorNoHit(1,1,1); // TODO use bkc from JSON instead
    Vector3f one(1,1,1);
    float minT1 = t1; // The minimum closest hit to viewer
    bool hitSomethingAtAll = false; // Should return bkg color if no hit

    // AMBIENT LIGHT
    // -------------
    float ka;
    Vector3f ai, ac;
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
                Eigen::Vector3f light_sum(0,0,0); // THE LIGHT SUM
                Eigen::Vector3f lightOutput; // THE FINAL OUTPUT
                // USE LOCAL ILLUMINATION FLAG
                // ---------------------------
                bool local_illum = true;
                // GLOBAL ILLUMINATION ACCUMULATOR
                // -------------------------------
                double light_sum_glob = 0.0;
                
                // LOCAL ILLUMINATION
                // ------------------
                if(local_illum) {

                    // ACCUMULATE LIGHT SOURCES
                    // ------------------------
                    for(Light* l : lights) {
                        light_sum += l->illuminate(ray, *closestHit);
                    }
                    // AMBIENT LIGHT CONTRIBUTION
                    // ------------------------
                    ka = closestHit->m->mat->ka; // Ambient coefficient
                    ac = closestHit->m->mat->ac; // Ambient color
                    Vector3f aL = ka * ac.cwiseProduct(ai);
                    light_sum += aL; // THE REFLECTION EQUATION
                    // CLAMP
                    // -----
                    lightOutput = clampVectorXf(light_sum, 0.0, 1.0);
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
                    // GLOBAL ILLUMINATION
                    // -------------------
                        
                    // Generate random points
                    // Build rays from these to the surface
                    // Accumulate light colors under certain
                    // constraints (i.e., max 5 bounces, etc.)
                    // Apply rejection zone
                    // Return ray to the eye...
                    light_sum_glob = getMonteCarloEstimator(1000);
                    cout<<"glob illum: "<<light_sum_glob<<endl;
                    Eigen::Vector3f lcol(0,0,1);
                    closestHit->color = clampVectorXf(light_sum_glob*lcol, 0.0, 1.0);
                }
                // ANTIALIASING OPTION
                // -------------------
                // Shoot more rays and take average
                // cout<<"Light sum:"<<light_sum<<endl;
                // cout<<"Light output:\n"<<lightOutput<<endl;
                //exit(0);
            }
        }
    }
  // If the ray has not touched any geometry at all, then use bkg color
  if(!hitSomethingAtAll) {
    hitReturn.color = colorNoHit;
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
float computePixelSizeDeltaHorizontal(float horizontal_fov, float height) {
  // 2*tan(fov/2)/width = 1 horizontal unit assuming square pixels (the ratio is already adjusted)
  return 2*tan(degToRad(horizontal_fov/2))/height;
};
float computePixelSizeVertical(float vertical_fov, float height) {
  return 2*tan(degToRad(vertical_fov/2))/height;
};
int RayTracer::save_ppm(const std::vector<float>& buffer, int dimx, int dimy) {
    for (unsigned int j = 0; j < dimy; j++) {
      for (unsigned int i = 0; i < dimx; i++) {
        ofs << (char) (255.0 * buffer[3*j*dimx+3*i + 0]) <<  (char) (255.0 * buffer[3*j*dimx+3*i + 1]) << (char) (255.0 * buffer[3*j*dimx+3*i+2]);
      }
    }
    std::cout<<"DONE!"<<std::endl;
    ofs.close();
    return 0;
};
void RayTracer::run() {
    cout<<"Running raytracer!"<<endl;
    if (!this->validateSceneJSONData()) {
      cout<<"Invalid scene! Aborting..."<<endl;
      return;
    }
    if (this->geometryRenderList.size() == 0) {
        cout<<"no geometry to render!"<<endl;
        return;
    }
    cout<<"N lights: "<<this->lights[0]<<endl;
    cout<<"Geo center: "<<this->geometryRenderList[0]->centre<<endl;
    // exit(0);

    // Camera setup: Build the view camera's axes and setup its parameters
    // ----------------
    float dimx = this->activeSceneCamera->size(0);
    float dimy = this->activeSceneCamera->size(1);
    float fov = this->activeSceneCamera->fov;
    float aspect_ratio = dimx/dimy;
    float horizontal_fov = fov;
    float vertical_fov = radToDeg(2*atan2(tan(degToRad(horizontal_fov/2)), aspect_ratio));
    float scale_v = tan(degToRad(vertical_fov/2));
    float scale_h = tan(degToRad(horizontal_fov/2));
    float MAX_RAY_DISTANCE = 1000;
    Vector3f cam_position = this->activeSceneCamera->centre;
    Vector3f cam_forward = this->activeSceneCamera->lookat; // already neg: -z
    Vector3f cam_up = this->activeSceneCamera->up;
    Vector3f cam_left = cam_forward.cross(cam_up);
    // Vector3f cam_left = -1*(cam_forward.cross(cam_up));
    cam_up = cam_forward.cross(cam_left); // Actual up
    cam_up *= -1; // flip it up
    
    // Use view matrix lookat
    cout<<"horizontal_fov: "<<horizontal_fov<<endl;
    cout<<"vertical_fov: "<<vertical_fov<<endl;
    cout<<"scale_v: "<<scale_v<<endl;
    cout<<"scale_h: "<<scale_h<<endl;
    cout<<"cam_position: "<<cam_position<<endl;
    cout<<"cam_forward (dir): "<<cam_forward<<endl;
    cout<<"cam_up: "<<cam_up<<endl;
    cout<<"cam_left: "<<cam_left<<endl;
    
    // Raycasting setup
    // ----------------
    float delta = computePixelSizeDeltaHorizontal(horizontal_fov, dimy); // The direct pixel size
    //float delta2 = computePixelSizeVertical(vertical_fov, dimy);
    Vector3f A = cam_position + (1)*cam_forward;
    Vector3f BA = A + (cam_up * (delta*(dimy/2)));
    Vector3f CB = BA - (cam_left * (delta*(dimx/2)));

    cout<<"size of a pixel: "<<delta<<endl;
    // cout<<"size vertically: "<<delta2<<endl;
    cout<<"BA (TOP_CENTER): "<<BA<<endl;
    cout<<"A CENTER: "<<A<<endl;
    cout<<"TOP_RIGHT: "<<CB<<endl;
    cout<<"DIMX: "<<dimx<<endl;
    cout<<"DIMY: "<<dimy<<endl;

    // Output buffer and output ppm file open
    // -------------
    std::vector<float> buffer = std::vector<float>(3*dimx*dimy);
    ofs.open("test_result.ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6" << endl << dimx << ' ' << dimy << endl << "255" << endl;

    // Raycasting other parameters
    // ---------------------
    Ray currentRay; // the current ray to raycast
    Vector3f ray_nds; // the raycasted ray in nds
    HitRecord* rayColor = new HitRecord(MAX_RAY_DISTANCE); // the ray color to use
    bool hitSomething = false; // if the current ray hit a geometry in the scene
    
    // exit(0);
    for(int j = dimy-1; j >= 0; j--) {
      for(int i = 0; i < dimx; i++) {
        // Screen space to NDC projection - TODO: Aspect ratio correction?
        // ------------------------------
        ray_nds = CB + (i*delta+delta/2)*cam_left - (j*delta+delta/2)*cam_up;
          
        // Modelview/Eye to World
        // ----------
        currentRay.setRay(cam_position, ray_nds);
          
        // Intersection Test
        // ----------
        // cout<<"ray nds: "<<ray_nds<<endl;
        hitSomething = groupRaycastHit(currentRay, 0, MAX_RAY_DISTANCE, *rayColor);

        // COLOR DEBUG 1
        // -----------
        // Vector3f tmp = currentRay.d.normalized();
          
        // Buffer out
        // ----------
        buffer[3*j*dimx+3*i+0] = rayColor->color(0);
        buffer[3*j*dimx+3*i+1] = rayColor->color(1);
        buffer[3*j*dimx+3*i+2] = rayColor->color(2);
        
        // COLOR DEBUG 2
        // -----------
//        buffer[3*j*dimx+3*i+0] = (tmp(0) + 1)/2;
//        buffer[3*j*dimx+3*i+1] = (tmp(1) + 1)/2;
//        buffer[3*j*dimx+3*i+2] = (tmp(2) + 1)/2;
      }
    }
    // Write to ppm
    // ----------
    save_ppm(buffer, dimx, dimy);
    delete rayColor;
};
