#include <string>
#include "AreaLight.h"
#include "RayTracer.h"

AreaLight::AreaLight(Vector3f P1, Vector3f P2, Vector3f P3, Vector3f P4, RayTracer* rt) {
    this->type = "area";
    this->P1 = P1;
    this->P2 = P2;
    this->P3 = P3;
    this->P4 = P4;
    this->rt = rt;
};
