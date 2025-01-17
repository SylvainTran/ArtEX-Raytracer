#pragma once

// External
#include <Eigen/Core>
#include <Eigen/Dense>

class RayTracer;
#include <vector>
using std::vector;

class JSONSceneParser {
public:
    /**
     * Constructors and destructors. 
     **/
    JSONSceneParser(nlohmann::json& j);
    ~JSONSceneParser();
    // Assignment and copy constructors
    JSONSceneParser(JSONSceneParser& other);
    JSONSceneParser& operator=(JSONSceneParser& other);
    // TODO: Operator overloads
    /**
     *  Tests the geometry.
     **/
    bool test_parse_geometry();
    /**
     *  Adds the geometry to the list in RayTracer.
     **/
    void parse_geometry(RayTracer* rt);
    /**
     *  Adds the output (camera parameters etc.) in the RayTracer.
     **/
    void parse_output(RayTracer* rt);
    /**
     *  Adds the lights to the list in RayTracer.
     **/
    void parse_lights(RayTracer* rt);
    /**
     *  Tests the lights. 
     **/
    bool test_parse_lights();
    /**
     *  Tests the output parameters. 
     **/
    bool test_parse_output();
    /**
     *  Copies the contents of a std::vector<float> into a Eigen::Vector3f array (from stl::vector<float> to eigen vector3f)
     **/
    void copyInputVector3f(int parameterLen, Eigen::Vector3f& centre, std::vector<float> input);
    /**
     *  Copies rectangle corners (from stl::vector<float> to eigen vector3f)
     **/
    void copyRectangleCorners(Eigen::Vector3f& P1, Eigen::Vector3f& P2, Eigen::Vector3f& P3, Eigen::Vector3f& P4, vector<float> p1_input, vector<float> p2_input, vector<float> p3_input, vector<float> p4_input);
private:
    /**
     *  The scene's data. 
     **/
    nlohmann::json sceneJSONData;
};
