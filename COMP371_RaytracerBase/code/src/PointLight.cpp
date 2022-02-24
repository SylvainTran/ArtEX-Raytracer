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
  }
  // LIGHT AND VIEW RAY INPUTS
  // ---------------------
  Eigen::Vector3f x = ray.evaluate(hrec.t);
  //cout<<"p: "<<p<<"x: "<<x<<endl;
  Vector3f light_ray = (p-x); // Light Ray: Ray from point x to PointLight
  Vector3f n = hrec.n;
  n = n.normalized();
  // ANGLES AND RAYS
  // ----
  Vector3f viewing_ray = -1*(ray.d);
  viewing_ray = viewing_ray.normalized();
  float cos_alpha, specular, specular_max, diffuse_max, distance, attenuation_factor;
  float cos_theta = max(n.dot(light_ray), 0.0);
  Vector3f lightColor(1,1,1);
  diffuse_max = 0;
  specular = 0;

  if(cos_theta > 0.0) {
    Vector3f reflection_ray = (2*n)*(cos_theta) - light_ray;
    // BLINN VERSION
    Vector3f half = viewing_ray + light_ray;
    half = half.normalized();
    //cos_alpha = n.dot(half);
    // PHONG VERSION 
    cos_alpha = reflection_ray.dot(viewing_ray);
    specular_max = max(0, cos_alpha);
    diffuse_max = max(0, cos_theta);
    distance = light_ray.norm() * light_ray.norm();
    attenuation_factor = 1 / distance; // Not used for now
    // LOCAL ILLUMINATION (BLINN-PHONG)
    // --------------------------------
    if(diffuse_max > 0) {
      specular = pow(specular_max, pc);
    }
  }
  //Vector3f diffuseColor = kd*(dc.cwiseProduct(lightColor))*diffuse_max;
  Vector3f diffuseColor = dc * diffuse_max;
  //diffuseColor = diffuseColor.cwiseProduct(id);
  //Vector3f specularColor = ks*(sc.cwiseProduct(lightColor))*specular;
  Vector3f specularColor = sc * specular;
  //specularColor = specularColor.cwiseProduct(is);
  return diffuseColor + specularColor;
};
