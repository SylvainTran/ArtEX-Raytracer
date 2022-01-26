#include "Triangle.h"

Triangle::Triangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3) {
  this->p1 = p1;
  this->p2 = p2;
  this->p3 = p3;
};
Triangle::~Triangle() {

};
HitRecord* Triangle::hit(Ray& r, float t0, float t1) {

};
void Triangle::info() {

};
