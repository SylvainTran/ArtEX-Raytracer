#pragma once
#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>

// My code
#include "Mesh.h"
#include "Ray.h"
#include "HitRecord.h"

struct Sphere : public Mesh {
	Sphere();
	Sphere(Eigen::Vector3f centre);
	~Sphere();
	Eigen::Vector3f centre;
	float radius;
	void info() override;
	// bool hit(Ray& r, float t0, float t1) override;
  HitRecord* hit(Ray& r, float t0, float t1) override;
  friend std::ostream& operator<<(std::ostream& ios, Sphere* s) {
	  ios<<"Sphere : centre="<<s->centre<<"\n, radius="<<s->radius<<std::endl;
	  return ios;
  };
};
