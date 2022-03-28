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

    Vector3f edgeX = (this->P2 - this->P1);
    Vector3f edgeY = (this->P4 - this->P1);

    float half_extent_x = (this->P3.x() + this->P1.x()) / 2; // midpoint
    float half_extent_z = (this->P3.z() + this->P1.z()) / 2;

    Vector3f center(half_extent_x, this->P3.y(), half_extent_z);

    bool outsideHorizontal = true, outsideVertical = true;

//    while(outsideHorizontal && outsideVertical) {
//        Utility::sampleOverUnitArea(intersected_point, n, rand_point, rand_dir_vec);
//        outsideHorizontal = std::abs(rand_dir_vec.x()) > std::abs(center.x() + half_extent_x) || std::abs(rand_dir_vec.x()) < std::abs(center.x() - half_extent_x);
//        outsideVertical = std::abs(rand_dir_vec.z()) > std::abs(center.z() + half_extent_z) || std::abs(rand_dir_vec.z()) < std::abs(center.z() - half_extent_z);
//        cout << "hor : " << outsideHorizontal << endl;
//        cout << "ver : " << outsideVertical << endl;
//    }
    //cout << "rand_point : " << rand_point << ", rand_dir_vec: " << rand_dir_vec << endl;
};
// Compute Light emitted (Le) vectors over the area light' area
// by sampling points inside the area's bounds
Vector3f AreaLight::illuminate(Ray& r, HitRecord& hrec) {
    Vector3f rand_point(0.f, 0.f, 0.f), rand_dir_vec(0.f, 0.f, 0.f), zero(0.f, 0.f, 0.f);
    const int N_SAMPLES = 5;

    std::vector<Vector3f> rand_points;

    for(int i = 0; i < N_SAMPLES; i++) {
        samplePointOverArea(r.evaluate(hrec.t), hrec.n, rand_point, rand_dir_vec);
        rand_points.push_back(rand_dir_vec);
    }

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

    float cos_theta, cos_alpha, shininess, distance, attenuation_factor;
    Vector3f diffuseColor, specularColor;
    Eigen::Vector3f x = r.evaluate(hrec.t);
    Vector3f n = (hrec.n).normalized();
    Vector3f viewing_ray = -1*(r.d);
    viewing_ray = viewing_ray.normalized();

    for(int i = 0; i < N_SAMPLES; i++) {

        Vector3f light_ray = (rand_points[i]-x); // Light Ray: Ray from point x in the area light's area
        light_ray = light_ray.normalized();

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

        diffuseColor += (kd * dc * cos_theta).cwiseProduct(id);
        specularColor += (ks * sc * shininess).cwiseProduct(is);
    }
    return diffuseColor + specularColor;
};
