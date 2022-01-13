#pragma once

// Includes
#include <iostream>
using std::cout;
using std::endl;
using std::vector;
#include <string>

// External Utilities
#include <Eigen/Core>
#include <Eigen/Dense>
#include "../external/json.hpp"

class JSONSceneParser {
public:
    /**
     * Constructors and destructors. 
     **/
    JSONSceneParser(nlohmann::json& j);
    ~JSONSceneParser();
    // TODO: Assignment and copy constructors
    // TODO: Operator overloads
    /**
     *  Tests the geometry. 
     **/
    bool test_parse_geometry();
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
