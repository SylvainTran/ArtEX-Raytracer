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

#include <Eigen/Core>
#include <Eigen/Dense>

#include "../external/json.hpp"
#include "../external/simpleppm.h"


class RayTracer {
public:
    RayTracer() {};
    RayTracer(nlohmann::json& j) {

    };
    ~RayTracer() {};  
    void run() {

    };
};