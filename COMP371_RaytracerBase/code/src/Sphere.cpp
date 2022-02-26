#include <iostream>
#include "Sphere.h"
#include "Ray.h"
#include "HitRecord.h"
#include "math.h"
using std::cout;
using std::endl;

Sphere::Sphere() {
	Eigen::Vector3f centre(0,0,0);
	this->centre = centre;
};
Sphere::Sphere(Eigen::Vector3f centre) {
    this->centre = centre;
    cout<<"Centre from constructor in sphere: "<<this->centre<<endl;
};
Sphere::~Sphere() {
	std::cout<<"Sphere destructor called!"<<std::endl;
};
bool Sphere::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
  
  Eigen::Vector3f e = r.o;
  Eigen::Vector3f d = r.d;
  Eigen::Vector3f c = this->centre;

  float discriminant = (d.dot(e-c)*d.dot(e-c) - (d.dot(d)*((e-c).dot(e-c) - this->radius*this->radius)));
  float s_t1, closest;

  if(discriminant<0) {
    return false;
  } else if (discriminant>=0) {
    s_t1 = (-1*(d.dot(e-c)) - sqrt(discriminant)) / (d.dot(d));
    closest = s_t1;
    //cout<<"closest (smallest abs) t: "<<closest<<endl;
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
