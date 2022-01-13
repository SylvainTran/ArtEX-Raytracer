#pragma once

// Standard Library includes
#include <iostream>
using std::cout;
using std::endl;
#include <string>
#include <memory>

// External Utilities
#include "../external/json.hpp"

// RayTracer and JSON scene parser
#include "RayTracer.h"
#include "JSONSceneParser.h"

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
private:
    /**
     *  The unique ptr to the raytracer.
     **/
    std::unique_ptr<RayTracer> rt;
    /**
     *  Reference to a JSONSceneParser
     **/
    std::unique_ptr<JSONSceneParser> jsp;    
};