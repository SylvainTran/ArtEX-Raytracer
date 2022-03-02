#pragma once
#include <iostream>

// My code
#include "Surface.h"
#include "Material.h"

// External
#include <Eigen/Core>
#include <Eigen/Dense>

struct Triangle : public Surface {
  /**The corners**/
  /**Actually, put in a matrix instead?**/
  Eigen::Vector3f p1;
  Eigen::Vector3f p2;
  Eigen::Vector3f p3;
  float t_hit; // the t of intersection if applicable
  Eigen::Vector3f normal_v;
  Material mat;

  /**The centre of the triangle**/
  //Eigen::Vector3f centre;
  /**Constructors and destructor**/
  Triangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3);
  ~Triangle();
  Triangle(Triangle& other) : mat(other.mat) {
    p1 = other.p1;
    p2 = other.p2;
    p3 = other.p3;
    color = other.color;
  };
  Triangle& operator=(Triangle& other) {
    p1 = other.p1;
    p2 = other.p2;
    p3 = other.p3;
    color = other.color;
    mat = other.mat;
    return *this;
  };
  /**Check for hit the PLANE!**/
  bool handleHitPlane(Ray& r, float t0, float t1);
  bool hit(Ray& r, float t0, float t1, HitRecord& hitReturn) override;
  /**Hit is inside the triangle**/
  bool hitInside();
  /**Info to log**/ 
  void info() override;
  /**Operator overrides**/ 
  friend std::ostream& operator<<(std::ostream& ios, Triangle* t) {
    ios<<"Triangle : p1="<<t->p1<<", p2="<<t->p2<<", p3="<<t->p3<<std::endl;
    return ios;
  };
};
