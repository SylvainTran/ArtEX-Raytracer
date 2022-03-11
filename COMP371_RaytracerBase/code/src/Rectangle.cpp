#include "Rectangle.h"
#include <vector>
#include <Eigen/Core>
#include "HitRecord.h"
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
bool Rectangle::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
     //std::cout<<"Rectangle hit:"<<std::endl;
     //exit(0);
    if(this->triangle1 == nullptr || this->triangle2 == nullptr) {
        std::cout<<"NULL triangles! Aborting hit..."<<std::endl;
        return false;
    }
    float t_hit;
    Eigen::Vector3f normal_v;
    Eigen::Vector3f color;
    HitRecord* ht1 = new HitRecord(t1);

    bool hitTriangle1 = this->triangle1->hit(r, t0, t1, *ht1);
    if(hitTriangle1) {
        //cout<<"Hit triangle 1"<<endl;
        t_hit = this->triangle1->t_hit;
        hitReturn.setNormal(this->triangle1->normal_v);
        hitReturn.setHitRecord(this, t_hit);
        return true;
    }
    HitRecord* ht2 = new HitRecord(t1);
    bool hitTriangle2 = this->triangle2->hit(r, t0, t1, *ht2);
    if(hitTriangle2) {
        //cout<<"Hit triangle 2"<<endl;
        t_hit = this->triangle2->t_hit;
        hitReturn.setNormal(this->triangle2->normal_v);
        hitReturn.setHitRecord(this, t_hit);
        return true;
    }
    return false;
};
Rectangle::~Rectangle() {

};
void Rectangle::makeRectangle() {

};
void Rectangle::info() {
  std::cout<<"I'm a rectangle."<<std::endl;
};
