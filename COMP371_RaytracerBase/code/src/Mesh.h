#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
#include "Ray.h"
#include "HitRecord.h"

struct Mesh {
	Mesh();
	~Mesh();
	Eigen::Vector3f centre;
	virtual void info();
  virtual HitRecord* hit(Ray& r, float t0, float t1);
  // virtual bool hit(Ray& r, float t0, float t1) = 0;
};
