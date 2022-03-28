#include <iostream>
#include "Surface.h"
#include "HitRecord.h"
#include "Sphere.h"
#include "BoundingBox.h"

Surface::Surface() : mat(new Material()), centre(0,0,0) {

};
Surface::Surface(Vector3f centre, BoundingBox* bbox) : centre(centre), bbox(bbox), mat(new Material() ){

};
Surface::Surface(Surface& surface) : bbox(surface.bbox), mat(surface.mat), centre(surface.centre), ignore(surface.ignore), type(surface.type), worldBounds(surface.worldBounds){

};
Surface::~Surface() {
//    delete mat;
//    delete worldBounds;
//    if(bbox) { // e.g., triangles don't have a bbox
//        //delete bbox;
//    }
};
Surface& Surface::operator=(Surface& surface) {
  this->mat = surface.mat;
  this->centre = surface.centre;
  this->ignore = surface.ignore;
  this->type = surface.type;
  this->worldBounds = surface.worldBounds;
  this->bbox = surface.bbox;
  return *this;
};
void Surface::info() {
	std::cout<<"Surface object :\nCentre: "<<centre<<std::endl;
};
bool Surface::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
//  std::cout<<"HIT ON SURFACE!"<<std::endl;
  return true;
}
void Surface::setbbox(BoundingBox* pBox) {
    this->bbox = pBox;
    cout << " set bbox! " << bbox << *bbox << endl;
};
//bool Surface::hit(Ray r, float t0, float t1) {
//  std::cout<<"HIT IN MESH!"<<std::endl;  
//};
