#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
#include "Surface.h"
#include <iostream>
using std::cout;
using std::endl;

struct HitRecord {
  Surface* m;
  float t;
  Eigen::Vector3f n;
  Eigen::Vector3f color;

  // Constructors and destructors
  // The constructor used to indicate a miss, by setting t = +inf
  HitRecord() : HitRecord(INFINITY) {
  };
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
    this->m = new Surface(*h.m);
    t = h.t;
    n = h.n;
    color = h.color;
  };
  ~HitRecord() {
    //delete this->m;
  };
  HitRecord& operator=(HitRecord& h) {
    this->m = h.m;
    t = h.t;
    n = h.n;
    color = h.color;
    return *this;
  };
  void setHitRecord(Surface* m, float t_hit, Eigen::Vector3f n, Eigen::Vector3f color) {
    this->m = m;
    this->t = t_hit;
    this->n = n;
    this->color = color;
  };
  void setHitRecord(Surface* m, float t_hit, Eigen::Vector3f& n) {
    this->m = m;
    this->t = t_hit;
    this->n = n;
  };
  void setHitRecord(Surface* m, float t_hit) {
    this->m = m;
    this->t = t_hit;
  };
  void setNormal(Eigen::Vector3f& n) {
    this->n = n;
  };
};
