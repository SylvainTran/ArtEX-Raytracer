#include "RayTracer.h"

RayTracer::RayTracer() {

};
RayTracer::RayTracer(nlohmann::json& j) {

};
RayTracer::~RayTracer() {

};
RayTracer& RayTracer::operator=(RayTracer& other) {
    // TODO: std::move() on other.members for this members'      
    return *this;
};
void RayTracer::run() {
    std::cout<<"Running raytracer!"<<std::endl;
    if(geometryRenderList.size() == 0) {
        std::cout<<"no geometry to render!"<<std::endl;        
        return;
    }
    std::cout<<"List of meshes to render:"<<std::endl;
    for(auto& m : geometryRenderList) {
        m->info();
        // TODO: pick up from here
    }
};