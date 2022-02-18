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
// TODO: Create a GROUP class?
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
      if(currentHit->t <= INFINITY) {
        //cout<<"rec->t is smaller than inf!"<<endl;
        hitSomethingAtAll = true;
        if(abs(currentHit->t) < abs(minT1)) {
          //cout<<"rec->t"<<currentHit->t<<" is smaller than min T1: "<<minT1<<endl;
          minT1 = currentHit->t;
          //cout<<"old t1: "<<t1<<" new t1: "<<minT1<<endl;
          t1 = minT1;
          closestHit = currentHit;
          // Lights
          ka = closestHit->m->mat->ka; // Ambient coefficient
          ac = closestHit->m->mat->ac; // Ambient color
          Vector3f aL = ka * ac.cwiseProduct(ai);
          Eigen::Vector3f light_sum(0,0,0);
          Eigen::Vector3f lightOutput = aL;
          
          for(Light* l : lights) {
            light_sum += l->illuminate(ray, *closestHit);
          }
          light_sum += aL;
          lightOutput = clampVectorXf(light_sum, 0.0, 1.0);
          cout<<"Light sum:"<<light_sum<<endl;
          cout<<"Light output:\n"<<lightOutput<<endl;
          closestHit->color = lightOutput;
          //exit(0);
        }
      }
    }
    currentHit->t = INFINITY;
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
  return 2*tan(horizontal_fov/2*(M_PI/180.0))/height;
};
float computePixelSizeVertical(float vertical_fov, float height) {
  return 2*tan(vertical_fov/2*(M_PI/180.0))/height;
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
    // For each pixel, create a ray (l) from the camera's origin to the center of the pixel
    // 1.1 Get the camera from the gE... shoot the ray from it
    float dimx = this->gE->activeSceneCamera->size(0);
    float dimy = this->gE->activeSceneCamera->size(1);
    float aspect_ratio = dimx/dimy;
    float fov = this->gE->activeSceneCamera->fov;
    float horizontal_fov = fov;
    float vertical_fov =  2*atan2(tan((horizontal_fov/2)*M_PI/180),aspect_ratio) * 180/M_PI; //vertical FOV in degrees
    float scale_v = tan((vertical_fov/2)*M_PI/180); // takes in the vertical fov instead of the fov directly (that one is only good for the horizontal fov)
    float scale_h = tan(horizontal_fov/2*M_PI/180);
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
    Eigen::Vector3f o = this->gE->activeSceneCamera->centre;
    cout<<"O: "<<o<<endl;
    Eigen::Vector3f dir = this->gE->activeSceneCamera->lookat;
    Eigen::Vector3f up = this->gE->activeSceneCamera->up;
    Eigen::Vector3f left = -1*(up.cross(dir)); // flip by -1 because dir is looking down the -z axis, and we're counting counterclockwise (right-handed system)
    cout<<"Dir: "<<dir<<endl;
    cout<<"Up: "<<up<<endl;
    cout<<"Left: "<<left<<endl;
    // exit(0);

    // POINTS ON THE PROJECTION PLANE
    // These are normalized coordinates (between -1 to 1)
    // Need perspective projection here to work normally
    Eigen::Vector3f A = o + (1)*dir;
    cout<<"A CENTER: "<<A<<endl;
    Eigen::Vector3f BA = A + (up * (delta*(dimy/2)));
    cout<<"BA (TOP_CENTER): "<<BA<<endl;
    Eigen::Vector3f CB = BA - (left * (delta*(dimx/2)));
    cout<<"TOP_RIGHT: "<<CB<<endl;
    cout<<"DIMX: "<<dimx<<endl;
    cout<<"DIMY: "<<dimy<<endl;
    std::vector<float> buffer = std::vector<float>(3*dimx*dimy);
    Ray currentRay;
    Eigen::Vector3f rayDirIJ;
    HitRecord* rayColor = new HitRecord(MAX_RAY_DISTANCE);
    bool hitSomething; 
    // Modelview representation
    // left, up, lookat concatenated with c of camera
    Eigen::Matrix<float,4,4> model_view {
      {left(0),0,0,o(0)},
      {0,up(1),0,o(1)},
      {0,0,dir(2),o(2)},
      {0,0,0,1}
    };
    //cout<<model_view<<endl;
    model_view = model_view.inverse().eval();
    //cout<<model_view<<endl;
    // Note: eigen has solve() and determinant() functions!
    // NDC
    // a = aspect_ratio
    // near_plane = n = parameter = 0.1;
    float a = aspect_ratio;
    float n = 0.1;
    // far_plane = f = cot(theta/2);
    float f = (1/tan(fov/2 * (M_PI/180.0f)));

    Eigen::Matrix<float,4,4> projection_transform{
      {f/a,0,0,0},
      {0,f,0,0},
      {0,0,(n+f)/(n-f),(2*(n*f))/(n-f)},
      {0,0,-1,0}
    };
    // cout<<projection_transform<<endl;
    // exit(0);
    // 1.2 Loop for every pixel, generate a new Ray for the pixel
    for(int j = dimy-1; j >= 0; j--) {
      for(int i = 0; i < dimx; i++) {
        // Update the ray R's coordinates at P(i,j) -> note that the ray space is not equal to the raster image pixel space
        rayDirIJ = CB + (i*delta+delta/2)*left - (j*delta+delta/2)*up;
        currentRay.setRay(o, (rayDirIJ-o));
        hitSomething = groupRaycastHit(currentRay, 0, MAX_RAY_DISTANCE, *rayColor);
        buffer[3*j*dimx+3*i+0] = rayColor->color(0);
        buffer[3*j*dimx+3*i+1] = rayColor->color(1);
        buffer[3*j*dimx+3*i+2] = rayColor->color(2);
      }
    }
    save_ppm(buffer, dimx, dimy);
    delete rayColor;
};
