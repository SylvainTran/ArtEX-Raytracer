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
#include "Surface.h"
#include "Sphere.h"
#include "Light.h"
#include "Camera.h"

class RayTracer {
  public:
    RayTracer();
    RayTracer(nlohmann::json& j);
    // Simple move constructor
    // RayTracer(RayTracer&& arg) { //: member(std::move(arg.member));
    // Simple assignment operator, TODO: move ass. operator
    RayTracer& operator=(RayTracer& other);
    ~RayTracer();  
    bool groupRaycastHit(Ray& ray, float t0, float t1, HitRecord& hitReturn);
    // Eigen::Vector3f groupRaycastHit(Ray* ray, float t0, float t1);
    bool validateSceneJSONData();
    void run();
    /**
     *  List of geometry (as meshes) to render in the scene.
     **/
    std::vector<Surface*> geometryRenderList;
    /**
     *  List of lights to render in the scene.
     **/
    std::vector<Light*> lights;
    int save_ppm(const std::vector<float>& buffer, int dimx, int dimy);
    Eigen::Vector3f clampVectorXf(Eigen::Vector3f value, float min, float max);
    /**
     * Adds a new mesh to the list of meshes to render.
     **/
    void addGeometry(Surface* s);
    /**
     * Gets the mesh in the list of meshes to render at index.
     **/
    void addLight(Light* l);
    Surface* getGeometry(int index);
    /**
     *  Camera(s)
     **/
    std::vector<Camera*> cameraList;
    /**
     *  Currently active camera in the scene window.
     **/
    Camera* activeSceneCamera;
    /**
     * Ambient intensity of the current scene.
     **/
    Eigen::Vector3f ambientIntensity;
    double getLambertianBRDF(float r);
    Eigen::Vector3f getAmbientReflectance(Vector3f ac, float ka);
    Vector3f getDiffuseReflection(HitRecord& hrec, Ray& ray, Vector3f x);
    Vector3f getSpecularReflectance(HitRecord& hrec, Ray& ray, Vector3f& n);
    void getBRDF2(Vector3f viewing_ray, Vector3f light_ray, Vector3f n);
    Vector3f getRandomVector(Ray& ray, Vector3f hitPoint, Vector3f n, Vector3f& random_dir_vector);
    Vector3f radiance(HitRecord& currentHit, Vector3f o, Vector3f d);
    // Space partitioning
    // ------------------
    // Fills a list of vertices representing voxel cubes in the viewing volume
    //
    // ------------------
    void partitionSpace(std::vector<float[]>& vertices, float screenWidth, float screenHeight, float near, float far);
    // Transformation => TODO: Refactor into camera
    // ------------------
    void modelViewTransformation(Eigen::Matrix4f& MVP, Surface& position);
    // Utility methods => TODO: Refactor into logger
    // ---------------
    void printUsefulLogs();
    void printUselessLogs(int dialogueNode);
    void printDebugLogs();
    void logSpeedTest(std::chrono::duration<double> time_span);
  private:
    nlohmann::json j;
};
