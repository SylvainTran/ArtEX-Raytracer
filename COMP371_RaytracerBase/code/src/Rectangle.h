#pragma once
#include <iostream>

// External
#include <Eigen/Core>
#include <Eigen/Dense>

// My code
#include "Surface.h"
#include "Triangle.h"

struct Rectangle : Surface {
  /** A rectangle in this model is made of two triangles**/
  Triangle* triangle1;
  Triangle* triangle2;
  /**Build the triangle using the corners/points provided**/
  Rectangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3, Eigen::Vector3f p4);
  //Rectangle(std::vector<Eigen::Vector3f> pointList);
  /**TODO: Assignment and copy constructors**/ 
	//Rectangle(Triangle* triangle1, Triangle* triangle2);
	~Rectangle();
  /**Builds the rectangle by putting the two triangles together**/
  void makeRectangle();
  /**Info to log**/ 
  void info() override;
  /**Operator overrides**/ 
  friend std::ostream& operator<<(std::ostream& ios, Rectangle* rect) {
    ios<<"Rectangle: triangle1="<<rect->triangle1<<"\ntriangle2="<<rect->triangle2<<std::endl;
    return ios;
  };
};
