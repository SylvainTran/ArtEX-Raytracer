// Standard
#include <iostream>
using std::cout;
using std::endl;

// My code
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
bool hit() {
    // 
    // 1.1 CHECK INTERSECTIONS
    // For each mesh/geometry in render list, check if any intersect with the ray
    // (l)
};
void RayTracer::run() {
    std::cout<<"Running raytracer!"<<std::endl;
    if (!this->gE->validateSceneJSONData(this->j)) {
      cout<<"Invalid scene! Aborting..."<<endl;
      return;
    }
    this->geometryRenderList = this->gE->geometryRenderList;
    if (this->geometryRenderList.size() == 0) {
        std::cout<<"no geometry to render!"<<std::endl;        
        return;
    }
    // 1. RAY GENERATION
    // //////////////////
    // For each pixel, create a ray (l) from the camera's origin to the center of 
    // the pixel
    // 
    // 1.1 Call the hit() method, which will loop for each Surface s in geometryRenderList and check for hits
    //
    int n = 0;
    std::cout<<"List of meshes to render:"<<std::endl;
    for(auto& m : this->geometryRenderList) {
      cout<<"n: "<<n<<endl;
      m->info();
      ++n;
	      // TODO: pick up from here
    }
};
