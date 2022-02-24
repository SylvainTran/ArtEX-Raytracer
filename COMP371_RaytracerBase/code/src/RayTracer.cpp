// Standard
#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
using std::ofstream;
#include <cstdio>
#include <vector>
#include <string>

// External
#include <Eigen/Geometry>
#include <math.h>       /* tan */

// My code
#include "HitRecord.h"
#include "RayTracer.h"
#include "GraphicsEngine.h"
#include "Triangle.h"
#include "Rectangle.h"
std::ofstream ofs;
using Eigen::Vector3f;
using Eigen::Vector4f;

RayTracer::RayTracer() {

};
RayTracer::RayTracer(nlohmann::json& j) {
  this->j = j;
  this->gE = new GraphicsEngine();
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
double integrate(int N) {
  double x, sum = 0.0f;
  for(int i = 0; i < N; i++) {
    x = drand48();
    sum += exp(sin(3*x*x));
  };
  return sum/double(N);
};
bool RayTracer::groupRaycastHit(Ray& ray, float t0, float t1, HitRecord& hitReturn) {
  HitRecord* currentHit = new HitRecord(INFINITY);
  HitRecord* closestHit = currentHit;
  Eigen::Vector3f colorNoHit(1,1,1); // TODO bkc from JSON instead

  float minT1 = t1;
  bool hitSomethingAtAll = false;

  // Ambient light
  float ka;
  Eigen::Vector3f ai, ac;
  ai = this->gE->ambientIntensity;
  
  for(Surface* s : this->geometryRenderList) {
    bool hitSomething = s->hit(ray, t0, t1, *currentHit);
    if(hitSomething) {
      //cout<<"rec->t "<<currentHit->t<<endl;
        //cout<<"rec->t is smaller than inf!"<<endl;
        hitSomethingAtAll = true;
        if(abs(currentHit->t) < abs(minT1)) {
          //cout<<"rec->t"<<currentHit->t<<" is smaller than min T1: "<<minT1<<endl;
          minT1 = currentHit->t;
          //cout<<"old t1: "<<t1<<" new t1: "<<minT1<<endl;
          t1 = minT1;
          closestHit = currentHit;
          // Lights
          Eigen::Vector3f light_sum(0,0,0);
          Eigen::Vector3f lightOutput;
          bool local_illum = true;
          double light_sum_glob;

          if(local_illum) {

            ka = closestHit->m->mat->ka; // Ambient coefficient
            ac = closestHit->m->mat->ac; // Ambient color
            Vector3f aL = ka * ac.cwiseProduct(ai);

            for(Light* l : lights) {
              light_sum = l->illuminate(ray, *closestHit);
            }
            light_sum += aL;
            lightOutput = clampVectorXf(light_sum, 0.0, 1.0);
            closestHit->color = lightOutput;
          } else {
            // Global illum
            // Generate random points
            // Build rays from these to the surface
            // Accumulate light colors under certain
            // constraints (i.e., max 5 bounces, etc.)
            // Apply rejection zone
            // Return ray to the eye...
            light_sum_glob = integrate(1000);
            cout<<"glob illum: "<<light_sum_glob<<endl;
            Eigen::Vector3f lcol(0,0,1);
            closestHit->color = clampVectorXf(light_sum_glob*lcol, 0.0, 1.0);
            // Antialiasing
            // Shoot more rays and take average
          }
          //lightOutput = light_sum;
          cout<<"Light sum:"<<light_sum<<endl;
          cout<<"Light output:\n"<<lightOutput<<endl;
          //exit(0);
        }
    }
    //currentHit->t = INFINITY;
  }
  // Ray has not touched any geometry at all, then give bg color
  if(!hitSomethingAtAll) {
    hitReturn.color = colorNoHit; // set to background color instead TODO
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
      std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
      for (unsigned int i = 0; i < dimx; i++) {
        ofs << (char) (255.0 * buffer[3*j*dimx+3*i + 0]) <<  (char) (255.0 * buffer[3*j*dimx+3*i + 1]) << (char) (255.0 * buffer[3*j*dimx+3*i+2]);
      }
    }
    std::cout<<"DONE!"<<std::endl;
    ofs.close();
    return 0;
};
void RayTracer::write_color(ofstream& ofs, HitRecord& pixel_color) {
  //std::cout<<"PIXEL COLOR: "<<pixel_color<<std::endl;
  int r = 255 * pixel_color.color(0);
  int g = 255 * pixel_color.color(1);
  int b = 255 * pixel_color.color(2);
  cout<<"r: "<<r<<",g: "<<g<<",b: "<<b<<endl;

  ofs << (char) r << ' '
      << (char) g << ' '
      << (char) b <<'\n';
};
void RayTracer::run() {
    cout<<"Running raytracer!"<<endl;
    if (!this->gE->validateSceneJSONData(this->j)) {
      cout<<"Invalid scene! Aborting..."<<endl;
      return;
    }
    this->geometryRenderList = this->gE->geometryRenderList;
    if (this->geometryRenderList.size() == 0) {
        cout<<"no geometry to render!"<<endl;
        return;
    }
    this->lights = this->gE->lights;
    cout<<"N lights: "<<this->lights[0]<<endl;
    // 1. RAY GENERATION
    float dimx = this->gE->activeSceneCamera->size(0);
    float dimy = this->gE->activeSceneCamera->size(1);
    float fov = this->gE->activeSceneCamera->fov;
    float aspect_ratio = dimx/dimy;
    float horizontal_fov = fov;
    float vertical_fov =  radToDeg(2*atan2(tan(degToRad(horizontal_fov/2)), aspect_ratio)); //vertical FOV in degrees
    float scale_v = tan(degToRad(vertical_fov/2)); // takes in the vertical fov instead of the fov directly (that one is only good for the horizontal fov)
    float scale_h = tan(degToRad(horizontal_fov/2));
    cout<<"horizontal_fov: "<<horizontal_fov<<endl;
    cout<<"vertical_fov: "<<vertical_fov<<endl;
    cout<<"scale_v: "<<scale_v<<endl;
    cout<<"scale_h: "<<scale_h<<endl;
    
    float MAX_RAY_DISTANCE = 1000;
    ofs.open("test_result8.ppm", std::ios_base::out | std::ios_base::binary);
    ofs << "P6" << endl << dimx << ' ' << dimy << endl << "255" << endl;
    
    // INITIAL POSITIONS
    float delta = computePixelSizeDeltaHorizontal(horizontal_fov, dimy);
    float delta2 = computePixelSizeVertical(vertical_fov, dimy);
    cout<<"size of a pixel: "<<delta<<endl;
    cout<<"size vertically: "<<delta2<<endl;
    //exit(0);
    
    // Build the view camera's axes
    Vector3f cam_position = this->gE->activeSceneCamera->centre;
    Vector3f cam_forward = this->gE->activeSceneCamera->lookat; // already neg: -z
    Vector3f cam_up = this->gE->activeSceneCamera->up;
    Vector3f cam_left = cam_forward.cross(cam_up);
    cam_up = cam_forward.cross(cam_left); // Actual up
    cam_up *= -1; // flip it up
    
    // Use view matrix lookat
    cout<<"cam_position: "<<cam_position<<endl;
    cout<<"cam_forward (dir): "<<cam_forward<<endl;
    cout<<"cam_up: "<<cam_up<<endl;
    cout<<"cam_left: "<<cam_left<<endl;
    
    // Modelview representation
    /* Uses the lookat view matrix */
    Eigen::Matrix<float,4,4> view_matrix {
        cam_left(0), cam_left(1), cam_left(2), -1*cam_left.dot(cam_position),
        cam_up(0), cam_up(1), cam_up(2), -1*cam_up.dot(cam_position),
        cam_forward(0), cam_forward(1), cam_forward(2), -1*cam_forward.dot(cam_position),
        0, 0, 0, 1
    };
    
    float a = aspect_ratio;
    float n = 0.1;
    float f = (1/tan(degToRad(fov/2)));

    Eigen::Matrix<float,4,4> projection {
      {f/a,0,0,0},
      {0,f,0,0},
      {0,0,(n+f)/(n-f),(2*(n*f))/(n-f)},
      {0,0,-1,0}
    };

    // POINTS ON THE PROJECTION PLANE
    // These are normalized coordinates (between -1 to 1)
    // Need perspective projection here to work normally
    Vector3f A = cam_position + (1)*cam_forward;
    Vector3f BA = A + (cam_up * (delta*(dimy/2)));
    Vector3f CB = BA - (cam_left * (delta*(dimx/2)));

    cout<<"BA (TOP_CENTER): "<<BA<<endl;
    cout<<"A CENTER: "<<A<<endl;
    cout<<"TOP_RIGHT: "<<CB<<endl;
    cout<<"DIMX: "<<dimx<<endl;
    cout<<"DIMY: "<<dimy<<endl;

    // Output buffer
    std::vector<float> buffer = std::vector<float>(3*dimx*dimy);
    // Raycasting parameters
    Ray currentRay;
    Vector3f ray_nds;
    HitRecord* rayColor = new HitRecord(MAX_RAY_DISTANCE);
    bool hitSomething;
    
    // exit(0);
    // 1.2 Loop for every pixel, generate a new Ray for the pixel
    for(int j = dimy-1; j >= 0; j--) {
      for(int i = 0; i < dimx; i++) {
        // Screen space to NDC projection
        // ------------------------------
        ray_nds = CB + (i*delta+delta/2)*cam_left - (j*delta+delta/2)*cam_up;
        Vector4f ray_nds_4f(ray_nds(0),ray_nds(1),ray_nds(2), 1);
        Vector4f ray_eye = projection.inverse() * ray_nds_4f;
 
        // Modelview/Eye to World
        // ----------
        Eigen::Matrix<float,4,4> ray_world = view_matrix.inverse().normalized() * ray_eye;
        Vector3f ray_world3f(ray_world(0,0), ray_world(1,1), ray_world(2,2));
        currentRay.setRay(cam_position, ray_world3f);
          
        //exit(0);
        hitSomething = groupRaycastHit(currentRay, 0, MAX_RAY_DISTANCE, *rayColor);
        // Buffer out
        buffer[3*j*dimx+3*i+0] = rayColor->color(0);
        buffer[3*j*dimx+3*i+1] = rayColor->color(1);
        buffer[3*j*dimx+3*i+2] = rayColor->color(2);
      }
    }
    save_ppm(buffer, dimx, dimy);
    delete rayColor;
};
