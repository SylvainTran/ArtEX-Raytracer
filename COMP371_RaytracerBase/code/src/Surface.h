#pragma once

#include <Eigen/Core>
#include <Eigen/Dense>
#include "Ray.h"
#include "Material.h"
class HitRecord;

struct Surface {
	Surface();
  Surface(Surface& surface);
  Surface& operator=(Surface& surface);
	~Surface();
	Eigen::Vector3f centre;
	Material* mat;
  virtual void info();
  virtual bool hit(Ray& r, float t0, float t1, HitRecord& hitReturn);
  // virtual bool hit(Ray& r, float t0, float t1) = 0;
};
