#pragma once
#include <iostream>

// My code
#include "Surface.h"

// External
#include <Eigen/Core>
#include <Eigen/Dense>

struct Triangle : Surface {
  /**The corners**/
  /**Actually, put in a matrix instead?**/
  Eigen::Vector3f p1;
  Eigen::Vector3f p2;
  Eigen::Vector3f p3;
  /**The centre of the triangle**/
  Eigen::Vector3f centre;
  /**Constructors and destructor**/
  Triangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3);
  ~Triangle();
  /**TODO: Copy and assignment constructors**/
  /**Check for hit**/
  HitRecord* hit(Ray& r, float t0, float t1) override;
  /**Info to log**/ 
  void info() override;
  /**Operator overrides**/ 
  friend std::ostream& operator<<(std::ostream& ios, Triangle* t) {
    ios<<"Triangle : centre="<<t->centre<<"\n"<<std::endl;
    return ios;
  };
};
