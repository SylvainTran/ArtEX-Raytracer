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
Sphere::Sphere(Eigen::Vector3f centre) : centre(centre) {

};
Sphere::~Sphere() {
	std::cout<<"Sphere destructor called!"<<std::endl;
};
bool Sphere::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
  // cout<<"In Sphere Hit!"<<endl;
  // cout<<"RAY COORDS: "<<r<<endl;
  
  Eigen::Vector3f e = r.o;
  Eigen::Vector3f d = r.d;
  Eigen::Vector3f c = this->centre;
  Eigen::Vector3f oc = e-c;
  float r2 = this->radius*this->radius;
  
  //auto a = d.dot(d);
  //auto b = 2.0 * oc.dot(d);
  //auto _c = oc.dot(oc) - r2;
  // -B +- sqrt(B^2 - 4AC)/2A
  //auto discriminant = b*b - 4*a*_c;
  float A = d.dot(d); // d.mag^2
  //float B = d.dot(e-c);
  float B = 2.0 * oc.dot(d);
  float C = (e-c).dot(e-c)-r2;
  float discriminant = B*B-4*(A*C);
  float closest, smallest, largest;
  float s_t1,s_t2;
  
  /**
  cout<<"This is for checking if the sphere's equation = 0"<<endl;
  cout<<"Meaning, can we find ts that make sense to solve the eqn?"<<endl;
  cout<<"Peter/textbook: "<<endl;
  cout<<"a: "<<a<<endl;
  cout<<"b: "<<b<<endl;
  cout<<"c: "<<_c<<endl;
  cout<<"DISCRIMINANT: "<<discriminant<<endl;
  cout<<"MY OWN: "<<endl;
  cout<<"A: "<<A<<endl;
  cout<<"B: "<<B<<endl;
  cout<<"C: "<<C<<endl;
  cout<<"discriminant2: "<<discriminant2<<endl;
  */
  //exit(0);
  
  if(discriminant<0) {
    //std::cout<<"Does not intersect!"<<std::endl;
    return false;
  } else if (discriminant==0) {
    s_t1 = -B/A;
    closest = s_t1;
    //cout<<"intersectedatedge!"<<endl;
    //cout<<closest<<endl;
    // exit(0);
  } else if (discriminant>0) {
    //cout<<"intersected!"<<endl;
    s_t1 = -B + sqrt(discriminant) * 1/2*A;
    s_t2 = -B - sqrt(discriminant) * 1/2*A;
    //cout<<"s_t1: "<<s_t1<<endl;
    //cout<<"s_t2: "<<s_t2<<endl;
    // exit(0);

    // 3 different cases:
    // Ray shot from the front of the sphere
    // Ray shot inside the sphere
    // Ray shot behind the sphere
    // cout<<"at: t = "<<s_t1<<", t = "<<s_t2<<endl;
    // exit(0);
    largest = s_t1>s_t2? s_t1 : s_t2;
    smallest = largest == s_t1? s_t2 : s_t1;
    if(smallest > t0 && smallest < t1) { // Inside the sphere and the ray has hit the backface?
      closest = smallest;
    } else if(largest > t0 && largest < t1) { // Ray hit the front face of the sphere since the smallest negative value (largest) is in the interval
      closest = largest;
    }
  }

  // std::cout<<"evaluating point at t for this ray: "<<r.evaluate(closest)<<std::endl;
  // exit(0);
  Eigen::Vector3f n = 2*(e+closest*d);
  Eigen::Vector3f color(0,0,1);
  //static HitRecord* hr = new HitRecord(this, closest, n, color);
  //std::cout<<"t value exiting sphere: "<<hr->t<<std::endl;
  //return hr; 
  hitReturn.setHitRecord(this, closest, n, color);
  return true;
};
void Sphere::info() {
	std::cout<<"Sphere object :\n"<<this<<std::endl;
};
