#include "PointLight.h"
#include "Ray.h"
#include "HitRecord.h"
using Eigen::Vector3f;
#include <iostream>
using std::cout;
using std::endl;
#include "GraphicsEngine.h"
#include <math.h>       /* pow */

PointLight::PointLight() {
};
PointLight::PointLight(Eigen::Vector3f centre, Eigen::Vector3f id, Eigen::Vector3f is, GraphicsEngine* gE) : p(centre) {
  this->id = id;
  this->is = is;
  this->I = id; // from JSON - not really used?
  this->gE = gE;
};
PointLight::~PointLight() {

};
float max(float a, float b) {
  return a>b?a:b;
};
float min(float a, float b) {
  return a<b?a:b;
};
float clamp(float val, float min, float max) {
  if(val < min) {
    val = min;
  }
  if(val > max) {
    val = max;
  }
  return val;
};
Eigen::Vector3f PointLight::illuminate(Ray& ray, HitRecord& hrec) {
  // SCENE PARAMETER INPUTS
  // ------------
  float kd, ks, pc;
  Vector3f dc, sc;
  if(hrec.m != nullptr && hrec.m->mat != NULL) { 
    kd = hrec.m->mat->kd; // Diffuse coefficient
    ks = hrec.m->mat->ks; // Specular coefficient
    dc = hrec.m->mat->dc; // Diffuse color
    sc = hrec.m->mat->sc; // Specular color
    pc = hrec.m->mat->pc; // Phong coefficient
  } else {
    kd = 1.0f;
  }
  // LIGHT AND VIEW RAY INPUTS
  // ---------------------
  Eigen::Vector3f x = ray.evaluate(hrec.t);
  Vector3f light_ray = (p-x); // Light Ray: Ray from point x to PointLight
  Vector3f l_norm = light_ray.normalized();
  Vector3f n = hrec.n;
  Vector3f n_norm = n.normalized();
  // ANGLES AND RAYS
  // ----
  Vector3f viewing_ray = -1*(ray.d).normalized();
  float cos_theta = clamp(n_norm.dot(l_norm), 0, 1);
  Vector3f reflection_ray = (((2*n_norm) * cos_theta) - l_norm);
  float cos_alpha = clamp(reflection_ray.dot(viewing_ray), 0, 1);
  float specular_max = max(0, cos_alpha);
  float diffuse_max = max(0, cos_theta);
  float distance = l_norm.norm() * l_norm.norm();
  float attenuation_factor = 1 / distance; // Not used for now
  //Vector3f k = hrec.m->mat->evaluate(l,-1*(ray.d/(ray.d.norm())),n_norm);
  // --------------------------------------------------------
  // I(lambda) = I(ambient lambda)*k(ambient) + f(att)*I(intensity power lambda)[k diffuse * (dot_product theta) + k specular * (dot_product alpha) ]
  // LOCAL ILLUMINATION (BLINN-PHONG)
  // --------------------------------
  float specular = 0;
  if(diffuse_max > 0) {
    specular = pow(specular_max, pc);
  }
  Vector3f lightColor(1,1,1);
  Vector3f diffuseColor = kd*(dc * lightColor.transpose())*diffuse_max;
  diffuseColor = diffuseColor.cwiseProduct(id);
  Vector3f specularColor = ks*(sc.cwiseProduct(lightColor))*specular;
  specularColor = specularColor.cwiseProduct(is);
  return diffuseColor + specularColor;
};
