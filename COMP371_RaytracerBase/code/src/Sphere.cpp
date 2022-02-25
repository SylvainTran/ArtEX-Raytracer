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
  Eigen::Vector3f oc = e-c;
  float r2 = this->radius*this->radius;
  float A = d.dot(d); // d.mag^2
  float B = 2.0 * oc.dot(d);
  float C = (e-c).dot(e-c)-r2;
  float discriminant = B*B-4*(A*C);
  float closest, smallest, largest,s_t1,s_t2;
  
  if(discriminant<0) {
    return false;
  } else if (discriminant==0) {
    s_t1 = -B/A;
    closest = s_t1;
  } else if (discriminant>0) {
    //cout<<"intersected!"<<endl;
    s_t1 = -B + sqrt(discriminant) * 1/2*A;
    s_t2 = -B - sqrt(discriminant) * 1/2*A;
    largest = s_t1>s_t2? s_t1 : s_t2;
    smallest = largest == s_t1? s_t2 : s_t1;
    if(smallest > t0 && smallest < t1) { // Inside the sphere and the ray has hit the backface?
      closest = smallest;
    } else if(largest > t0 && largest < t1) { // Ray hit the front face of the sphere since the smallest negative value (largest) is in the interval
      closest = largest;
    }
  }
  Eigen::Vector3f n = oc; // TEMP, TO BE ACTUALLY SET FROM PT OF INTERSECTION INSTEAD
  Eigen::Vector3f color = this->mat->ac;
  hitReturn.setHitRecord(this, closest, n, color);
  return true;
};
void Sphere::info() {
	std::cout<<"Sphere object :\n"<<this<<std::endl;
};
