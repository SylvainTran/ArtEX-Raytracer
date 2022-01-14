#pragma once
#include "iostream"

// RayTracer and JSON scene parser
#include "RayTracer.h"
class JSONSceneParser;
class Mesh;

class GraphicsEngine {
    /**
     * This class could be a module in a larger game engine.
     * It contains a smart ptr to a raytracer.
     **/
public:
    /**
     *  Constructors and destructors.
     **/
    GraphicsEngine();
    ~GraphicsEngine();
    // TODO: Assignment and copy constructors
    // TODO: Operator overloads
    /**
     *  Makes a unique raytracer on demand.
     **/
    std::unique_ptr<RayTracer> RayTracerFactory();
    /**
     *  Validates the scene json data.
     **/
    nlohmann::json validateSceneJSONData(const char* argv);
    /**
     * Sets/gets a new active raytracer.
     **/
    void setActiveRayTracer(RayTracer& rt);
    RayTracer getActiveRayTracer();
    /**
     * Adds a new mesh to the list of meshes to render.
     **/
    void addGeometry(Mesh* s);
    /**
     * Gets the mesh in the list of meshes to render at index.
     **/
    Mesh* getGeometry(int index);
private:
    /**
     *  The raytracer.
     **/
    RayTracer rt;
    /**
     *  Reference to a JSONSceneParser
     **/
    std::unique_ptr<JSONSceneParser> jsp; 
    /**
     *  List of geometry (as meshes) to render in the scene.
     **/
    std::vector<Mesh*> geometryRenderList;   
};