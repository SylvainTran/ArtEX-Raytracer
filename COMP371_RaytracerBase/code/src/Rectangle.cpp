#include "Rectangle.h"
#include <vector>
#include <Eigen/Core>

Rectangle::Rectangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3, Eigen::Vector3f p4) {

};
/**
Rectangle::Rectangle(std::vector<Eigen::Vector3f> pointList) {

};
*/
Rectangle::Rectangle(Triangle* triangle1, Triangle* triangle2) {
  this->triangle1 = triangle1;
  this->triangle2 = triangle2;
};
Rectangle::~Rectangle() {

};
void Rectangle::makeRectangle() {

};
void Rectangle::info() {

};
