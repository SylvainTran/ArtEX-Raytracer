#include "Rectangle.h"
#include <vector>
#include <Eigen/Core>
#include "HitRecord.h"
#include "BBox3D.h"
#include "BoundingBox.h"
using namespace std;

Rectangle::Rectangle(Vector3f p1, Vector3f p2, Vector3f p3, Vector3f p4) : triangle1(new Triangle(p1, p2, p3)),  triangle2(new Triangle(p1,p3, p4)), Surface(Vector3f(0,0,0), new BoundingBox()) {
    bbox = new BoundingBox(this);
};
/**
Rectangle::Rectangle(std::vector<Eigen::Vector3f> pointList) {

};
*/
bool Rectangle::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
    //std::cout<<"Rectangle hit:"<<std::endl;
//    cout << *bbox << endl;

    if(bbox != nullptr && !bbox->hit(r, t0, t1, hitReturn)) {
        return false;
    }

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

    //hitReturn.setHitRecord(this, INFINITY);
    return false;
};
Rectangle::~Rectangle() {

};
void Rectangle::makeRectangle() {

};
void Rectangle::info() {
  std::cout<<"I'm a rectangle."<<std::endl;
};
