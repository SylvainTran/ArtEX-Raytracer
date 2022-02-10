#include "Triangle.h"
#include "HitRecord.h"
using namespace std;

Triangle::Triangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3) {
  this->p1 = p1;
  this->p2 = p2;
  this->p3 = p3;
  this->color = Eigen::Vector3f(0,1,0);
};
Triangle::~Triangle() {

};
bool Triangle::handleHitPlane(Ray& r, float t0, float t1) {
  // did it hit the plane?
  // compute t
  float M;
  float beta;
  float grass;
  float t;
  float a,b,c,d,e,f,g,h,i,j,k,l;
  // std::cout<<"computing t inside Triangle::handleHitPlane!: "<<std::endl;
  // Vector ab on the plane
  a = this->p1(0) - this->p2(0);
  b = this->p1(1) - this->p2(1);
  c = this->p1(2) - this->p2(2);
  // Vector ac on the plane 
  d = this->p1(0) - this->p3(0);
  e = this->p1(1) - this->p3(1);
  f = this->p1(2) - this->p3(2);
  // The ray's components
  g = r.d(0);
  h = r.d(1);
  i = r.d(2);
  // Direction vector between point A and the ray's origin
  j = this->p1(0) - r.o(0);
  k = this->p1(1) - r.o(1);
  l = this->p1(2) - r.o(2);
  // Matrix A's determinant
  M = a*(e*i-h*f) + b*(g*f-d*i) + c*(d*h-e*g);
  // Intersection point 
  t = (-1*(f*(a*k-j*b) + e*(j*c-a*l) + d*(b*l - k*c))) * 1/M;
  
  /* Summary
  cout<<"a: "<<a<<endl;
  cout<<"b: "<<b<<endl;
  cout<<"c: "<<c<<endl;
  cout<<"d: "<<d<<endl;
  cout<<"e: "<<e<<endl;
  cout<<"f: "<<f<<endl;
  cout<<"g: "<<g<<endl;
  cout<<"h: "<<h<<endl;
  cout<<"i: "<<i<<endl;
  cout<<"j: "<<j<<endl;
  cout<<"k: "<<k<<endl;
  cout<<"l: "<<l<<endl;
  cout<<"M: "<<M<<endl;
  cout<<"t: "<<t<<endl;
  */
  //float alpha = 1-beta-grass;
  //cout<<"Alpha: "<<alpha<<endl;
  // exit(0);
  if(t < t0 || t > t1) {
    //cout<<"T out of range! : "<<t<<endl;
    return false;
  }
  grass = (i*(a*k-j*b) + h*(j*c-a*l) + g*(b*l - k*c)) * 1/M;
  //cout<<"Grass: "<<grass<<endl;
  if(grass < 0 || grass > 1) {
    //cout<<"Grass out of range! : "<<grass<<endl;
    return false;
  }
  beta = (j*(e*i-h*f) + k*(g*f-d*i) + l*(d*h-e*g)) * 1/M;
  if(beta < 0 || beta > (1-grass)) {
    //cout<<"Beta out of range! : "<<beta<<endl;
    return false;
  }
  //cout<<"beta: "<<beta<<endl;
  this->t_hit = t;
  //std::cout<<"Valid hit for triangle! "<<this->t_hit<<std::endl;

  bool hitInsideTriangle = beta > 0 && grass > 0 && (beta + grass < 1); 
  //exit(0);
  return hitInsideTriangle;
};
bool Triangle::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
 bool hit = handleHitPlane(r,t0,t1);
 if(!hit) {
  return false;
 }
 //std::cout<<"Hit: "<<hit<<std::endl;
 Eigen::Vector3f n = (p2-p1).cross(p3-p1);
 
 //std::cout<<"HIT TRIANGLE! "<<this<<std::endl;
 //static HitRecord* hr = new HitRecord(this, this->t_hit, n, this->color);
 //return hr;
 hitReturn.setHitRecord(this, this->t_hit, n, this->color);
 return true;
};
void Triangle::info() {

};
