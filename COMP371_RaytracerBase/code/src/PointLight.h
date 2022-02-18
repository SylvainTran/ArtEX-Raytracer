#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>

#include "Light.h"
#include "GraphicsEngine.h"

struct PointLight : public Light {
  Eigen::Vector3f I;
  Eigen::Vector3f p;
  GraphicsEngine* gE;  
  PointLight();
  PointLight(Eigen::Vector3f centre, Eigen::Vector3f id, Eigen::Vector3f is, GraphicsEngine* gE);
	~PointLight();
  Eigen::Vector3f illuminate(Ray& r, HitRecord& hrec) override;
};
