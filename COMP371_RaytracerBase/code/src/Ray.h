#pragma once
#include <math.h>
#include <Eigen/Core>
#include <Eigen/Dense>

struct Ray {
  /**
   * The origin point of the ray.
   */
  Eigen::Vector3f o;
  /**
   * This is the direction vector of the ray, usually given as the lookat vec3f.
   */
  Eigen::Vector3f d;
  // Constructors and destructor
  Ray(Eigen::Vector3f o, Eigen::Vector3f d) {
    this->o = o;
    this->d = d;
  };
  ~Ray() {
  };
  // Returns a point at o+t(d)
  Eigen::Vector3f evaluate(float t) {
    return o+(t*d);
  };
  Eigen::Vector3f normalize() {
    return this->d * (1/sqrt((d(0)*d(0)+d(1)*d(1)+d(2)*d(2))));
  };
  friend std::ostream& operator<<(std::ostream& out, Ray& ray) {
    out << "Ray origin: "<<ray.o<<"\nRay dir: "<<ray.d<<std::endl;
    return out;
  };
};

