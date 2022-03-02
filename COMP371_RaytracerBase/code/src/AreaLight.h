#include <Eigen/Core>
#include "Light.h"
#include "RayTracer.h"
using Eigen::Vector3f;

struct AreaLight : public Light {
    Vector3f P1,P2,P3,P4;
    RayTracer* rt = rt;
	AreaLight();
    AreaLight(Vector3f P1, Vector3f P2, Vector3f P3, Vector3f P4, RayTracer* rt);
	~AreaLight();
};
