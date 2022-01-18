#include <iostream>
#include "Sphere.h"
#include "Ray.h"
#include "HitRecord.h"
#include "math.h"

Sphere::Sphere() {
	Eigen::Vector3f centre(0,0,0);
	this->centre = centre;
};
Sphere::Sphere(Eigen::Vector3f centre) : centre(centre) {

};
Sphere::~Sphere() {
	std::cout<<"Sphere destructor called!"<<std::endl;
};
HitRecord* Sphere::hit(Ray r, float t0, float t1) {
  Eigen::Vector3f d = r.d;
  Eigen::Vector3f c = this->centre;
  Eigen::Vector3f e = r.o;
  float s_t1,s_t2;
  float r2 = this->radius*this->radius;
  
  // -B +- sqrt(B^2 - 4AC)/2A
  float A = d.dot(d); // d.mag^2
  float B = d.dot(e-c);
  float C = (e-c).dot(e-c)-r2;
  float discriminant = B*B-(A*C);
  float closest, smallest, largest;
  bool intersect;
  
  if(discriminant<0) {
    intersect = false;
  } else if (discriminant==0) {
    s_t1 = -B/A;
  } else if (discriminant>0) {
    s_t1 = (-B + sqrt((B*B)*(B*B)-A*C))/A;
    s_t2 = (-B - sqrt((B*B)*(B*B)-A*C))/A;
    largest = s_t1>s_t2? s_t1 : s_t2;
    smallest = largest == s_t1? s_t2 : s_t1;
    if(smallest > t0 && smallest < t1) {
      closest = smallest;
    } else if(largest > t0 && largest < t1) {
      closest = largest;
    }
  }
  Eigen::Vector3f n = 2*(e+closest*d);
  //HitRecord(Mesh* m, float t, Eigen::Vector3f n)
  //return new HitRecord(); 
};
void Sphere::info() {
	std::cout<<"Sphere object :\n"<<std::endl;
	std::cout<<this<<std::endl;
};
