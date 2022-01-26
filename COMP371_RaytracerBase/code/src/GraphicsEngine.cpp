// Standard
#include <iostream>
#include <string>
#include <memory>
using std::cout;
using std::endl;
#include <vector>
using std::vector;

// External Utilities
#include "../external/json.hpp"

// My code
#include "RayTracer.h"
#include "Camera.h"
#include "GraphicsEngine.h"
#include "JSONSceneParser.h"
#include "Surface.h"

GraphicsEngine::GraphicsEngine() {

};
GraphicsEngine::~GraphicsEngine() {};
std::unique_ptr<RayTracer> GraphicsEngine::RayTracerFactory() {
    return std::make_unique<RayTracer>();
};
bool GraphicsEngine::validateSceneJSONData(nlohmann::json j) {
    this->jsp = new JSONSceneParser(this, j);
    if(!jsp->test_parse_geometry() || !jsp->test_parse_lights() || !jsp->test_parse_output()) {
      cout<<"One of the tests failed. Aborting..."<<endl;
      return false;
    } else {
      cout<<"parsing geometry!"<<endl;
      jsp->parse_geometry(this);
      cout<<"parsing output (camera, etc.)!"<<endl;
      jsp->parse_output(this);
    }
    return true;
};
void GraphicsEngine::setActiveRayTracer(RayTracer& rt) {
    this->rt = rt;
};
RayTracer GraphicsEngine::getActiveRayTracer() {
    return this->rt;
};
void GraphicsEngine::addGeometry(Surface* s) {
    // this->geometryRenderList.push_back(s);
    // TODO: notify observers (UI editor? multiple renderers?)
    this->geometryRenderList.push_back(s);
    std::cout<<"Added geometry to render list!"<<std::endl;
};
Surface* GraphicsEngine::getGeometry(int index) {
    //return this->geometryRenderList[index];
    return this->rt.geometryRenderList[index];
};
