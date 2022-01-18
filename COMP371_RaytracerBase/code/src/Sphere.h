#pragma once
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

// My code
#include "Mesh.h"
class Ray;
class HitRecord;

struct Sphere : public Mesh {
	Sphere();
	Sphere(Eigen::Vector3f centre);
	~Sphere();
	Eigen::Vector3f centre;
	float radius;
	void info() override;
	HitRecord* hit(Ray r, float t0, float t1);
  friend std::ostream& operator<<(std::ostream& ios, Sphere* s) {
	  ios<<"Sphere : centre="<<s->centre<<"\n, radius="<<s->radius<<std::endl;
	  return ios;
  };
};
