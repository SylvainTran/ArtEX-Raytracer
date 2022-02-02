#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
class Surface;

struct HitRecord {
  Surface* m;
  float t;
  Eigen::Vector3f n;
  Eigen::Vector3f color;

  // Constructors and destructors
  // The constructor used to indicate a miss, by setting t = +inf
  HitRecord(float t) {
    this->t = t;
    this->m = nullptr;
    this->n = Eigen::Vector3f(0,0,0);
    this->color = Eigen::Vector3f(0,0,0);
  };
  HitRecord(Surface* m, float t, Eigen::Vector3f n, Eigen::Vector3f c) {
    this->m = m;
    this->t = t;
    this->n = n;
    this->color = c;
  };
  HitRecord(HitRecord& h) {
    this->m = h.m;
    t = h.t;
    n = h.n;
    color = h.color;
  };
  ~HitRecord() {
  
  };
  HitRecord& operator=(HitRecord& h) {
    this->m = h.m;
    t = h.t;
    n = h.n;
    color = h.color;
    return *this;
  };
};
