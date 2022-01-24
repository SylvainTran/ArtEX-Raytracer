#include <iostream>
#include "Mesh.h"
#include "HitRecord.h"

Mesh::Mesh() {

};
Mesh::~Mesh() {

};
void Mesh::info() {
	std::cout<<"Mesh object :\nCentre: "<<centre<<std::endl;
};
HitRecord* Mesh::hit(Ray& r, float t0, float t1) {
  std::cout<<"HIT IN MESH!"<<std::endl;  
};
//bool Mesh::hit(Ray r, float t0, float t1) {
//  std::cout<<"HIT IN MESH!"<<std::endl;  
//};
