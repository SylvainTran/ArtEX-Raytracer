#include "PointLight.h"
#include "Ray.h"
#include "HitRecord.h"
using Eigen::Vector3f;
#include <iostream>
using std::cout;
using std::endl;
#include "RayTracer.h"
#include <math.h>       /* pow */

PointLight::PointLight() {
};
PointLight::PointLight(Eigen::Vector3f centre, Eigen::Vector3f id, Eigen::Vector3f is, RayTracer* gE) : p(centre) {
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
    // ----------------------
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
    // LIGHT AND VIEW RAY INPUTS (NORMAL AND POINT OF INTERSECTION)
    // ------------------------------------------------------------
    Eigen::Vector3f x = ray.evaluate(hrec.t);
    Vector3f light_ray = (p-x); // Light Ray: Ray from point x to PointLight
    light_ray = light_ray.normalized();
    Vector3f n = hrec.n;
    
    // OPTIMIZATIONS
    // -------------
    float epsilon_approx = 0.00000000000001f;
    if(light_ray.dot(x) <= epsilon_approx) {
        // light is parallel to the point of intersection on the surface
    }
    
    // ANGLES AND RAYS SETUP
    // ---------------------
    Vector3f viewing_ray = -1*(ray.d);
    viewing_ray = viewing_ray.normalized();
    float cos_theta, cos_alpha, shininess, distance, attenuation_factor;

    // LOCAL ILLUMINATION (BLINN-PHONG)
    // --------------------------------
    cos_theta = max(n.dot(light_ray), 0.0);
    shininess = 0.0;
    
    // BLINN VERSION
    // -------------
    Vector3f half = viewing_ray + light_ray;
    half = half.normalized();
    cos_alpha = max(0.0, n.dot(half));
    // ------------------------
    // PHONG VERSION
    // -------------
    Vector3f reflection_ray = (2*n)*(cos_theta) - light_ray;
    // cos_alpha = reflection_ray.dot(viewing_ray);
    // Clamp to positive values and calculate attenuation factor
    // ---------------------------------------------------------
    distance = light_ray.norm() * light_ray.norm();
    attenuation_factor = 1 / distance; // Not used for now

    // SPECULAR HIGHLIGHT (SHININESS)
    // ------------------
    if(cos_theta > 0.0) {
        shininess = pow(cos_alpha, pc);
    }
    // OUTPUT COLOR FOR DIFFUSE + SPECULAR
    // -----------------------------------
    Vector3f lightColor(1,1,1);
    Vector3f diffuseColor = (kd * dc * cos_theta).cwiseProduct(id).cwiseProduct(lightColor);
    Vector3f specularColor = (ks * sc * shininess).cwiseProduct(is).cwiseProduct(lightColor);
    
    // NORMALS DEBUG
    // -------------
    // return n;
    //
    // COLOR DEBUG
    // -----------
    // return diffuseColor;
    //
    // SPECULAR DEBUG
    // -----------
    // return specularColor;
    //
    // OUTPUT DEBUG
    // -----------
    return diffuseColor + specularColor;
};
