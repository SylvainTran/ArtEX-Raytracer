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
#include <fstream>
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
#include "HitRecord.h"
#include "Ray.h"
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
    HitRecord* groupRaycastHit(Ray* ray, float t0, float t1);
    // Eigen::Vector3f groupRaycastHit(Ray* ray, float t0, float t1);
    void run();
    /**
     *  List of geometry (as meshes) to render in the scene.
     **/
    std::vector<Mesh*> geometryRenderList;
    int save_ppm(std::string file_name, const std::vector<float>& buffer, int dimx, int dimy);
    // void write_color(std::ofstream& ofs, Eigen::Vector3f color);
    void write_color(std::ofstream& ofs, HitRecord& hit);
  private:
    nlohmann::json j;
    GraphicsEngine* gE;
};
