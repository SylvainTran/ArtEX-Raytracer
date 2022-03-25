#include <iostream>
#include "Sphere.h"
#include "Ray.h"
#include "HitRecord.h"
#include "math.h"
#include "Utility.hpp"
using std::cout;
using std::endl;

Sphere::Sphere() {
	Eigen::Vector3f centre(0,0,0);
	this->centre = centre;
};
Sphere::Sphere(Eigen::Vector3f centre) {
    this->centre = centre;
};
Sphere::~Sphere() {

};
bool Sphere::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
    float A, B, C, r2, s_t1, s_t2, closest;
    Eigen::Vector3f e = r.o;
    Eigen::Vector3f d = r.d;
    Eigen::Vector3f c = this->centre;
    B = d.dot(e-c);

    // Z-tests to compare front/back
    bool camBehind = (std::abs(e(2)) > std::abs(c(2)));
    bool camInFront = (std::abs(e(2)) < std::abs(c(2)));

    // Return false if shooting ray away from sphere (dot > 0 since eye and sphere are looking at the same dir)
    if (camBehind && B > 0) {
        return false;
    }

    r2 = this->radius*this->radius;
    A = d.dot(d); // 1
    C = ((e-c).dot(e-c) - r2);
    float discriminant = B*B - A*C;

    if(discriminant < 0) {
        return false;
    } else if(discriminant == 0) {
        return B / A;
    } else if (discriminant > 0) {
        s_t1 = (-B - sqrt(discriminant)) / A; // more negative back hit
        s_t2 = (-B + sqrt(discriminant)) / A;

        if(std::abs(s_t1) < std::abs(t0) || std::abs(s_t2) > std::abs(t1)) {
            return false;
        }
        if (Utility::squaredDistance(e, c) < r2) {
            closest = s_t1; // take back hit
        } else if (camInFront && B > 0) { // ray hits in front of sphere
            closest = s_t2;
        } else { // ray hits back of sphere
            closest = s_t1;
        }
    }
    // NORMAL
    // ------
    Eigen::Vector3f n = r.evaluate(closest) - c;
    n = n.normalized();
    Eigen::Vector3f color = this->mat->ac;
    hitReturn.setHitRecord(this, closest, n, color);
    //cout<<"t: "<< closest<<endl;
    return true;
};
void Sphere::info() {
	std::cout<<"Sphere object :\n"<<this<<std::endl;
};
