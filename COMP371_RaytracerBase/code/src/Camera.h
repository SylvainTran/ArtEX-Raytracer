#pragma once

#include <Eigen/Dense>
#include <iostream>

struct Camera {
  float fov;
  Eigen::Vector3f centre;
  Eigen::Vector3f lookat;
  Eigen::Vector3f up;
  Eigen::Vector3f size;
  Camera() {
    std::cout<<"New camera constructed!"<<std::endl;
  };
  Camera(float fov, Eigen::Vector3f centre, Eigen::Vector3f lookat, Eigen::Vector3f up, Eigen::Vector3f size) :
    fov(fov), centre(centre), lookat(lookat), up(up), size(size)
  {
    std::cout<<"New camera constructed!"<<std::endl;
  };
  ~Camera() {
    std::cout<<"Camera destroyed!"<<std::endl;
  };
};
