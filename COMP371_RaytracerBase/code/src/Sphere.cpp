#include <iostream>
#include "Sphere.h"

Sphere::Sphere() {
	Eigen::Vector3f centre(0,0,0);
	this->centre = centre;
};
Sphere::Sphere(Eigen::Vector3f centre) : centre(centre) {

};
Sphere::~Sphere() {
	std::cout<<"Sphere destructor called!"<<std::endl;
};
void Sphere::info() {
	std::cout<<"Sphere object :\n"<<std::endl;
	std::cout<<this<<std::endl;
};