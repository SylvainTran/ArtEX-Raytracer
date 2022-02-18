#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
using Eigen::Vector3f;

struct Material {
  Vector3f l, v, n;
  Vector3f ac,dc,sc;
  float ka, kd, ks, pc;
  Material();
  Material(Material& mat);
  Material& operator=(Material& mat);
  Material(Eigen::Vector3f l, Eigen::Vector3f v, Eigen::Vector3f n);
  ~Material();
  Eigen::Vector3f evaluate(Eigen::Vector3f l, Eigen::Vector3f v, Eigen::Vector3f n);
};
