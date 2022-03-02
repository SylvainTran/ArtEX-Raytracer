#pragma once

#include <Eigen/Dense>
#include <iostream>
#include <string>

struct Camera {
  float fov;
  Eigen::Vector3f centre;
  Eigen::Vector3f lookat;
  Eigen::Vector3f up;
  Eigen::Vector3f size;
  Eigen::Vector3f bkc;
  std::string filename;
  
  Camera() {

  };
  Camera(float fov, Eigen::Vector3f centre, Eigen::Vector3f lookat, Eigen::Vector3f up, Eigen::Vector3f size) :
    fov(fov), centre(centre), lookat(lookat), up(up), size(size)
  {

  };
  ~Camera() {

  };
};
