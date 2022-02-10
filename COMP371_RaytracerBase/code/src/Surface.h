#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
#include "Ray.h"
class HitRecord;

struct Surface {
	Surface();
	~Surface();
	Eigen::Vector3f centre;
	virtual void info();
  virtual bool hit(Ray& r, float t0, float t1, HitRecord& hitReturn);
  // virtual bool hit(Ray& r, float t0, float t1) = 0;
};
