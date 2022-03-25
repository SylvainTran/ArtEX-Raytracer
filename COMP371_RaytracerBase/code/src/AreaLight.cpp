#include <string>
#include "AreaLight.h"
#include "RayTracer.h"
#include "Utility.hpp"
using Eigen::Vector3f;

AreaLight::AreaLight(Vector3f P1, Vector3f P2, Vector3f P3, Vector3f P4, RayTracer* rt) {
    this->type = "area";
    this->P1 = P1;
    this->P2 = P2;
    this->P3 = P3;
    this->P4 = P4;
    this->rt = rt;
}

Vector3f AreaLight::samplePointOverArea(Vector3f intersected_point, Vector3f n, Vector3f& rand_point, Vector3f& rand_dir_vec) {

    Vector3f edgeX = this->P1 - this->P2;
    Vector3f edgeY = this->P1 - this->P4;

    // TODO ; get this right
    float half_extent_x = (1/2) * edgeX.x();
    float half_extent_y = (1/2) * edgeY.y();
    // Vector3f center((edgeX / 2).x(), (edgeY / 2).y(), 0.0f);
    Vector3f center((this->P1 + edgeX * half_extent_x).x(), (this->P1 + edgeY * half_extent_y).y(), 0.0f);

    cout << "edgex : " << edgeX << ", edgeY : " << edgeY << endl;
    cout << " center : " << center << endl;

    bool outsideHorizontal = true, outsideVertical = true;

//    while(outsideHorizontal && outsideVertical) {
//        Utility::sampleOverUnitArea(intersected_point, n, rand_point, rand_dir_vec);
//        //cout << "rand_point : " << rand_point << ", rand_dir_vec: " << rand_dir_vec << endl;
//        outsideHorizontal = rand_point.x() > center.x() + half_extent_x || rand_point.x() < center.x() - half_extent_x;
//        outsideVertical = rand_point.y() > center.y() + half_extent_y || rand_point.y() < center.y() - half_extent_y;
//    }
    cout <<" SAMPLED AREA LIGHT PT OK" << endl;
};
// Compute Light emitted (Le) vectors over the area light' area
// by sampling points inside the area's bounds
Vector3f AreaLight::illuminate(Ray& r, HitRecord& hrec) {
    Vector3f rand_point, rand_dir_vec;
    samplePointOverArea(r.evaluate(hrec.t), hrec.n, rand_point, rand_dir_vec);

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

    Eigen::Vector3f x = r.evaluate(hrec.t);
    Vector3f light_ray = (rand_point-x); // Light Ray: Ray from point x in the area light's area
    light_ray = light_ray.normalized();
    Vector3f n = hrec.n;

    Vector3f viewing_ray = -1*(r.d);
    viewing_ray = viewing_ray.normalized();
    float cos_theta, cos_alpha, shininess, distance, attenuation_factor;

    cos_theta = Utility::max(n.dot(light_ray), 0.0);
    shininess = 0.0;

    Vector3f half = viewing_ray + light_ray;
    half = half.normalized();
    cos_alpha = Utility::max(0.0, n.dot(half));

    Vector3f reflection_ray = (2*n)*(cos_theta) - light_ray;
    // cos_alpha = reflection_ray.dot(viewing_ray);

    distance = light_ray.norm() * light_ray.norm();
    attenuation_factor = 1 / distance; // Not used for now

    if(cos_theta > 0.0) {
        shininess = pow(cos_alpha, pc);
    }

    Vector3f diffuseColor = (kd * dc * cos_theta).cwiseProduct(id);
    Vector3f specularColor = (ks * sc * shininess).cwiseProduct(is);

    return diffuseColor + specularColor;
};
