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
#include "GraphicsEngine.h"
#include "JSONSceneParser.h"
#include "RayTracer.h"
#include "Mesh.h"

GraphicsEngine::GraphicsEngine() {
    // auto newRayTracer = RayTracerFactory();
    // rt = std::move(newRayTracer);
};
GraphicsEngine::~GraphicsEngine() {};
std::unique_ptr<RayTracer> GraphicsEngine::RayTracerFactory() {
    return std::make_unique<RayTracer>();
};
nlohmann::json GraphicsEngine::validateSceneJSONData(const char* argv) {
    cout<<"Scene: "<<argv[1]<<endl;    
    std::ifstream t(argv);
    if(!t){
        cout<<"File "<<argv[1]<<" does not exist!"<<endl;
        return -1;
    }    
    std::stringstream buffer;
    buffer << t.rdbuf();   
    cout<<"Parsed successfuly"<<endl;
    nlohmann::json j = nlohmann::json::parse(buffer.str());
    // Run tests
   	JSONSceneParser jsp = JSONSceneParser(this, j);
    if(!jsp.test_parse_geometry() || !jsp.test_parse_lights() || !jsp.test_parse_output()) {
        cout<<"One of the tests failed. Aborting..."<<endl;
        return NULL;
    }
    return j;
};
void GraphicsEngine::setActiveRayTracer(RayTracer& rt) {
    this->rt = rt;
};
RayTracer GraphicsEngine::getActiveRayTracer() {
    return this->rt;
};
void GraphicsEngine::addGeometry(Mesh* s) {
    // this->geometryRenderList.push_back(s);
    // TODO: notify observers (multiple renderers?)
    this->rt.geometryRenderList.push_back(s);
    std::cout<<"Added geometry to render list!"<<std::endl;
};
Mesh* GraphicsEngine::getGeometry(int index) {
    //return this->geometryRenderList[index];
    return this->rt.geometryRenderList[index];
};