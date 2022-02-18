#pragma once
#include <iostream>

// External
#include <Eigen/Core>
#include <Eigen/Dense>

// My code
#include "Surface.h"
#include "Triangle.h"
#include "Material.h"

struct Rectangle : public Surface {
  /**A rectangle in this model is made of two triangles**/
  Triangle* triangle1;
  Triangle* triangle2;
  /**Build the triangle using the corners/points provided**/
  Rectangle(Eigen::Vector3f P1, Eigen::Vector3f P2, Eigen::Vector3f P3, Eigen::Vector3f P4);
  //Rectangle(std::vector<Eigen::Vector3f> pointList);
  /**TODO: Assignment and copy constructors**/ 
	Rectangle(Triangle* triangle1, Triangle* triangle2);
	~Rectangle();
  /**Builds the rectangle by putting the two triangles together**/
  void makeRectangle();
  bool hit(Ray& r, float t0, float t1, HitRecord& hitReturn) override;
  /**Info to log**/ 
  void info() override;
  /**Operator overrides**/ 
  friend std::ostream& operator<<(std::ostream& ios, Rectangle rect) {
    ios<<"Rectangle: triangle1="<<rect.triangle1<<"\ntriangle2="<<rect.triangle2<<std::endl;
    return ios;
  };
};
