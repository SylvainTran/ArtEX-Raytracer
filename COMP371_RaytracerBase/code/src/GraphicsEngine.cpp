#include "GraphicsEngine.h"
#include "JSONSceneParser.h"
#include <memory>

GraphicsEngine::GraphicsEngine() {
    auto newRayTracer = RayTracerFactory();
    rt = std::move(newRayTracer);
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
   	JSONSceneParser jsp = JSONSceneParser(j);
    if(!jsp.test_parse_geometry() || !jsp.test_parse_lights() || !jsp.test_parse_output()) {
        cout<<"One of the tests failed. Aborting..."<<endl;
        return NULL;
    }
    return j;
};