#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>

#include "Light.h"
#include "RayTracer.h"

struct PointLight : public Light {
    Eigen::Vector3f I;
    Eigen::Vector3f p;
    RayTracer* gE;
    PointLight();
    PointLight(Eigen::Vector3f centre, Eigen::Vector3f id, Eigen::Vector3f is, RayTracer* gE);
    ~PointLight();
    Eigen::Vector3f illuminate(Ray& r, HitRecord& hrec) override;
};
