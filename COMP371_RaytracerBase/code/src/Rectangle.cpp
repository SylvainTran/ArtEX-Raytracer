#include "Rectangle.h"
#include <vector>
#include <Eigen/Core>
using namespace std;

Rectangle::Rectangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3, Eigen::Vector3f p4) {

};
Rectangle::Rectangle(Triangle* triangle1, Triangle* triangle2) {
  this->triangle1 = new Triangle(*triangle1);
  this->triangle2 = new Triangle(*triangle2);
};
/**
Rectangle::Rectangle(std::vector<Eigen::Vector3f> pointList) {

};
*/
HitRecord* Rectangle::hit(Ray& r, float t0, float t1) {
 //std::cout<<"Rectangle hit:"<<std::endl;
 if(this->triangle1 == nullptr || this->triangle2 == nullptr) {
   //std::cout<<"NULL triangles! Aborting hit..."<<std::endl;
   return nullptr;
 }
 HitRecord* hitTriangle1 = this->triangle1->hit(r,t0,t1);
 HitRecord* hitTriangle2 = this->triangle2->hit(r,t0,t1);
 Eigen::Vector3f color;

 if(hitTriangle1 == nullptr && hitTriangle2 == nullptr) {
  return nullptr;
 }
 bool hitAny = hitTriangle1 != nullptr || hitTriangle2 != nullptr;
 //std::cout<<"Hit triangle 1 or 2 (the rect) : "<<hitAny<<std::endl;
 if(!hitAny) {
   //std::cout<<"Did not Hit triangle 1 or 2!"<<std::endl;
   //exit(0);
   return nullptr;
 }
 float t_hit;
 Eigen::Vector3f normal_v;
 if(hitTriangle1) {
   cout<<"Hit triangle 1"<<endl;
   t_hit = this->triangle1->t_hit;
   //exit(0);
   color = hitTriangle1->color;
   normal_v = this->triangle1->normal_v;
 }
 if(hitTriangle2) {
   //cout<<"Hit triangle 2"<<endl;
   t_hit = this->triangle2->t_hit;
   normal_v = this->triangle2->normal_v;
   color = hitTriangle2->color;
   //exit(0);
 }
 //std::cout<<"HIT RECT: "<<std::endl;
 static HitRecord* hr = new HitRecord(this, t_hit, normal_v, color);
 return hr;
};
Rectangle::~Rectangle() {

};
void Rectangle::makeRectangle() {

};
void Rectangle::info() {
  std::cout<<"I'm a rectangle."<<std::endl;
};
