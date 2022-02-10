#include <iostream>
#include "Surface.h"
#include "HitRecord.h"

Surface::Surface() {

};
Surface::~Surface() {

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
