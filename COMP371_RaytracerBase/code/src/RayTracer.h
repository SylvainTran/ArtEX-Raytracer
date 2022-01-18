#pragma once
////////////////////////
/**
 * 
 * Author: Sylvain Tran
 * Program Description: A simple raytracer for COMP 371.
 * Date Created: Tue, Jan. 11th, 2022
 * Date Last Updated: Tue, Jan. 11th, 2022
 * 
 */
////////////////////////
#include <iostream>
#include <string>
#include <memory>
#include <vector>
using std::cout;
using std::endl;

#include <Eigen/Core>
#include <Eigen/Dense>

#include "../external/json.hpp"
#include "../external/simpleppm.h"

// My code

class GraphicsEngine;
#include "Mesh.h"
#include "Sphere.h"

class RayTracer {
  public:
    RayTracer();
    RayTracer(nlohmann::json& j);
    // Simple move constructor
    // RayTracer(RayTracer&& arg) { //: member(std::move(arg.member));
    // Simple assignment operator, TODO: move ass. operator
    RayTracer& operator=(RayTracer& other);
    ~RayTracer();  
    void run();
    /**
     *  List of geometry (as meshes) to render in the scene.
     **/
    std::vector<Mesh*> geometryRenderList; 
  private:
    nlohmann::json j;
    GraphicsEngine* gE;
};
