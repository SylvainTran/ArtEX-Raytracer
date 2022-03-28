#include <iostream>
#include "Sphere.h"
#include "Ray.h"
#include "HitRecord.h"
#include "math.h"
#include "Utility.hpp"
#include "BoundingBox.h"
using std::cout;
using std::endl;

Sphere::Sphere() {

};
Sphere::Sphere(Eigen::Vector3f centre) : Surface(centre, bbox) {
    bbox = new BoundingBox(this);
};
Sphere::Sphere(Sphere& sphere) : Surface(sphere.centre, new BoundingBox()) {
    this->radius = sphere.radius;
    bbox = new BoundingBox(this);
};
Sphere& Sphere::operator=(Sphere& sphere) {
    radius = sphere.radius;
    return *this;
};
Sphere::~Sphere() {

};
bool Sphere::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
    float A, B, C, r2, s_t1, s_t2, closest;
    Vector3f e = r.o, d = r.d, c = this->centre;

    // Z-tests to compare front/back
    bool camBehind = (std::abs(e(2)) > std::abs(c(2)));
    bool camInFront = (std::abs(e(2)) < std::abs(c(2)));

    B = d.dot(e-c);

    // Return false if shooting ray away from sphere (dot > 0 since eye and sphere are looking at the same dir)
    if (camBehind && B > 0) {
        hitReturn.setHitRecord(this, INFINITY);
        return false;
    }

    r2 = this->radius*this->radius;
    A = d.dot(d); // 1
    C = ((e-c).dot(e-c) - r2);
    float discriminant = B*B - A*C;

    if(discriminant < 0) {
        hitReturn.setHitRecord(this, INFINITY);
        return false;
    } else if(discriminant == 0) {
        closest = B / A;
    } else if (discriminant > 0) {
        s_t1 = (-B - sqrt(discriminant)) / A; // more negative back hit
        s_t2 = (-B + sqrt(discriminant)) / A;

        if (Utility::squaredDistance(e, c) < r2) {
            closest = s_t1; // take back hit
        } else if (camInFront && B > 0) { // ray hits in front of sphere
            closest = s_t2;
        } else { // ray hits back of sphere
            closest = s_t1;
        }
    }

    Vector3f n = r.evaluate(closest) - c;
    //n = n.normalized(); // DON'T NORMALIZE THIS AUTOMATICALLY, MAY NEED UN-NORMALIZED ONE OTHERWISE
    Vector3f color = this->mat->ac;
    hitReturn.setHitRecord(this, closest, n, color);
    return true;
};
void Sphere::info() {
	std::cout<<"Sphere object :\n"<<this<<std::endl;
};
