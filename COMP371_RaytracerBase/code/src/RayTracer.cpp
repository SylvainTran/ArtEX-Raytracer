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
// TODO: Create a GROUP class?
HitRecord* RayTracer::groupRaycastHit(Ray* ray, float t0, float t1) {
  static HitRecord* closestHit = new HitRecord(INFINITY); // TODO: I made this static but may want to use shared_ptr instead?
  Eigen::Vector3f colorHit(0,1,0);
  Eigen::Vector3f colorNoHit(1,1,1);

  for(auto& m : this->geometryRenderList) {
    HitRecord* rec = m->hit(*ray, t0, t1);
    if(rec == NULL) {
      closestHit->color = colorNoHit;
    }
    else {
      closestHit = rec;
      closestHit->color = colorHit;
      t0 = closestHit->t;
      cout<<"Hit at t0="<<t0<<endl;
    }
    // m->info();
  }
  // return color;
  return closestHit;
};
// Computes the size of a pixel
float computePixelSizeDelta(float fov, float height) {
  // we don't assume that height = width, so we need to use the ratio (width/height) too
  // 2*tan(fov/2)/height * ratio = 1 vertical unit
  // 2*tan(fov/2)/width = 1 horizontal unit
  return 2*tan(fov/2*(M_PI / 180.0))/height;
};
int RayTracer::save_ppm(std::string file_name, const std::vector<float>& buffer, int dimx, int dimy) {
    ofstream ofs(file_name, std::ios_base::out | std::ios_base::binary);
    ofs << "P6" << endl << dimx << ' ' << dimy << endl << "255" << endl;
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
}
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
    // 1. RAY GENERATION
    // For each pixel, create a ray (l) from the camera's origin to the center of the pixel
    // 1.1 Get the camera from the gE... shoot the ray from it
    float dimx = this->gE->activeSceneCamera->size(0);
    float dimy = this->gE->activeSceneCamera->size(1);
    float ratio = dimx/dimy;
    float MAX_RAY_DISTANCE = 1000;
    Ray* R;
    // INITIAL POSITIONS
    float delta = computePixelSizeDelta(this->gE->activeSceneCamera->fov, dimy);
    cout<<"size of a pixel: "<<delta<<endl;
    Eigen::Vector3f o = this->gE->activeSceneCamera->centre;
    cout<<"O: "<<o<<endl;
    Eigen::Vector3f dir = this->gE->activeSceneCamera->lookat;
    Eigen::Vector3f up = this->gE->activeSceneCamera->up;
    Eigen::Vector3f right = up.cross(dir);
    cout<<"Dir: "<<dir<<endl;
    cout<<"Up: "<<up<<endl;
    cout<<"Right: "<<right<<endl;

    // POINTS ON THE PROJECTION PLANE
    // These are normalized coordinates (between -1 to 1)
    Eigen::Vector3f A = o + (1)*dir;
    cout<<"A CENTER: "<<A<<endl;
    Eigen::Vector3f BA = A + up * (tan((this->gE->activeSceneCamera->fov/2)*M_PI/180)) * ratio; // Then it will be the index j, since max(j) = height in pixels, each j => j/max ratio to scale
    cout<<"BA (TOP_CENTER): "<<BA<<endl;
    Eigen::Vector3f CB = BA - right * (delta*dimx/2) ; // Then it will be index i since max(i) = width in pixels
    cout<<"TOP_LEFT: "<<CB<<endl;
    cout<<"DIMX: "<<dimx<<endl;
    cout<<"DIMY: "<<dimy<<endl;
    std::vector<float> buffer = std::vector<float>(3*dimx*dimy);
    
    // 1.2 Loop for every pixel, generate a new Ray for the pixel
    for(int j = dimy-1; j >= 0; j--) {
      for(int i = 0; i < dimx; i++) {
        // Update the ray R's coordinates at P(i,j) -> note that the ray space is not equal to the raster image pixel space
        Eigen::Vector3f rayDirIJ = CB + (j*delta+delta/2)*right - (i*delta+delta/2)*up;
        HitRecord* rayColor = groupRaycastHit(new Ray(o, (rayDirIJ-o)), 0, MAX_RAY_DISTANCE);
        buffer[3*j*dimx+3*i+0] = rayColor->color(0);
        buffer[3*j*dimx+3*i+1] = rayColor->color(1);
        buffer[3*j*dimx+3*i+2] = rayColor->color(2);

        // QUESTION FOR OFFICE HOURS: I DONT NEED TO EVAL THE RAY POINT TO COLOR IT. JUST THE RAY DIR? PROVIDED BY I J DIMX DIMY ALREADY IT SEEMS
        // Pixel position i,j in the raster image VS. ray pixel center?
      }
    }
    save_ppm("test_scene1_result3.ppm", buffer, dimx, dimy);
};

//<- Graveyard ->

//Eigen::Vector3f lower_left_corner = o - Eigen::Vector3f(dimx/2, dimy/2, 0) - Eigen::Vector3f(0, 0, -1);
//auto u = double(i) / (dimx-1);
//auto v = double(j) / (dimy-1);

//cout<<"RAY TO COLOR : "<<(rayDirIJ-o)<<endl;
// 1.3 Call the hit() method, which will loop for each Surface s in geometryRenderList and check for hits
// HitRecord* rayColor = groupRaycastHit(new Ray(o, (lower_left_corner + u*right + v*up - o)), 0, MAX_RAY_DISTANCE);
