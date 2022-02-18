#include <iostream>
#include "Surface.h"
#include "HitRecord.h"

Surface::Surface() {
  this->mat = new Material();
  Eigen::Vector3f c(0,0,0);
  this->centre = c;
};
Surface::Surface(Surface& surface) {
  this->mat = surface.mat;
  this->centre = surface.centre;
};
Surface::~Surface() {

};
Surface& Surface::operator=(Surface& surface) {
  this->mat = surface.mat;
  this->centre = surface.centre;
  return *this;
};
void Surface::info() {
	std::cout<<"Surface object :\nCentre: "<<centre<<std::endl;
};
bool Surface::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
  std::cout<<"HIT ON SURFACE!"<<std::endl;
  return true;
};
//bool Surface::hit(Ray r, float t0, float t1) {
//  std::cout<<"HIT IN MESH!"<<std::endl;  
//};
