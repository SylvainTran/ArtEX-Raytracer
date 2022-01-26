#pragma once
#include "iostream"

// RayTracer and JSON scene parser
#include "RayTracer.h"
#include "Camera.h"
class JSONSceneParser;
class Surface;

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
    bool validateSceneJSONData(nlohmann::json j);
    /**
     * Sets/gets a new active raytracer.
     **/
    void setActiveRayTracer(RayTracer& rt);
    RayTracer getActiveRayTracer();
    /**
     * Adds a new mesh to the list of meshes to render.
     **/
    void addGeometry(Surface* s);
    /**
     * Gets the mesh in the list of meshes to render at index.
     **/
    Surface* getGeometry(int index);
    /**
     *  The raytracer.
     **/
    RayTracer rt;
    /**
     *  Reference to a JSONSceneParser
     **/
    JSONSceneParser* jsp; 
    /**
     *  List of geometry (as surfaces) to render in the scene.
     **/
    std::vector<Surface*> geometryRenderList;   
    /**
     *  Camera(s)
     **/
    std::vector<Camera*> cameraList;
    /**
     *  Currently active camera in the scene window.
     **/
    Camera* activeSceneCamera;
};
