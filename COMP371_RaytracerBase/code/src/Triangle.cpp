#include <Eigen/Core>
#include "Triangle.h"
#include "HitRecord.h"
using namespace std;
using Eigen::Vector3f;

Triangle::Triangle(Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f p3) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    // SET NORMAL
    // ----------
    this->normal_v = (this->p2 - this->p1).cross(this->p3 - this->p1);
//    this->normal_v =  normal_v.normalized();
};
Triangle::~Triangle() {

};
// Simple solution
bool Triangle::handleHitPlane(Ray& r, float t0, float t1) {
    // build plane
    Vector3f edgeAB = p2 - p1;
    Vector3f edgeAC = p3 - p1;
    Vector3f n = edgeAB.cross(edgeAC);
    this->normal_v = n; //n.normalized();
    float q1 = r.d.dot(n);

    // check if ray is parallel to plane
    if(std::abs(q1) < 0.0001f) {
        return false;
    }

    // Plane
    Vector3f pointOnPlane = p1;

    float q0 = n.dot(pointOnPlane - r.o);
    float t = q0/q1;

    this->t_hit = t;
    if(std::abs(t) < std::abs(t0) || std::abs(t) > std::abs(t1)) {
        return false;
    }

    Vector3f intersect_x = r.o + t*r.d;

    // Within triangle test
    // edgeAB;
    Vector3f edgeBC = p3 - p2;
    Vector3f edgeCA = p1 - p3;

    Vector3f AtoPoint = intersect_x - p1;
    Vector3f BtoPoint = intersect_x - p2;
    Vector3f CtoPoint = intersect_x - p3;

    Vector3f Atest = edgeAB.cross(AtoPoint); // + == side of normal
    Vector3f Btest = edgeBC.cross(BtoPoint);
    Vector3f Ctest = edgeCA.cross(CtoPoint);

    return Atest.dot(n) > 0.f && Btest.dot(n) > 0.f && Ctest.dot(n) > 0.f;
};
// Uses cramer's rule - from textbook
// FIXME: the t value
bool Triangle::barycentricHitTriangle(Ray& r, float t0, float t1) {
    float M,beta,gamma,t,a,b,c,d,e,f,g,h,i,j,k,l,xa,xb,xc,xd,xe,ya,yb,yc,yd,ye,za,zb,zc,zd,ze;

    xa = this->p1(0);
    xb = this->p2(0);
    xc = this->p3(0);
    xd = r.d(0);
    ya = this->p1(1);
    yb = this->p2(1);
    yc = this->p3(1);
    yd = r.d(1);
    za = this->p1(2);
    zb = this->p2(2);
    zc = this->p3(2);
    zd = r.d(2);

    // Eye
    xe = r.o(0);
    ye = r.o(1);
    ze = r.o(2);

    Eigen::Matrix3f t_mat;
    t_mat << xa-xb, xa-xc, xa-xe,
            ya-yb, ya-yc, ya-ye,
            za-zb, za-zc, za-ze;
    Eigen::Matrix3f a_mat;
    a_mat << xa-xb, xa-xc, xd,
            ya-yb, ya-yc, yd,
            za-zb, za-zc, zd;

    // std::cout<<"computing t inside Triangle::handleHitPlane!: "<<std::endl;
    // Vector ab on the plane
    a = this->p1(0) - this->p2(0);
    b = this->p1(1) - this->p2(1);
    c = this->p1(2) - this->p2(2);
    // Vector ac on the plane
    d = this->p1(0) - this->p3(0);
    e = this->p1(1) - this->p3(1);
    f = this->p1(2) - this->p3(2);
    // The ray's components
    g = r.d(0);
    h = r.d(1);
    i = r.d(2);
    // Direction vector between point A and the ray's origin
    j = this->p1(0) - r.o(0);
    k = this->p1(1) - r.o(1);
    l = this->p1(2) - r.o(2);

    // Matrix A's determinant
    M = a*(e*i-h*f) + b*(g*f-d*i) + c*(d*h-e*g);

    // Intersection point
    t = (-1*(f*(a*k-j*b) + e*(j*c-a*l) + d*(b*l - k*c))) * 1/M;
    //t = t_mat.determinant() / a_mat.determinant();

    if(std::abs(t) < t0 || std::abs(t) > t1) return false;
    gamma = (i*(a*k-j*b) + h*(j*c-a*l) + g*(b*l - k*c)) * 1/M;

    if(gamma < 0 || gamma > 1) return false;
    beta = (j*(e*i-h*f) + k*(g*f-d*i) + l*(d*h-e*g)) * 1/M;

    if(beta < 0 || beta > (1-gamma)) return false;
    this->t_hit = t;

    //std::cout<<"Valid hit for triangle! "<<this->t_hit<<std::endl;
    return beta > 0 && gamma > 0 && (beta + gamma < 1);
};
bool Triangle::hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
    //bool hit = handleHitPlane(r,t0,t1);
    bool hit = barycentricHitTriangle(r,t0,t1);
    if(!hit) return false;
    hitReturn.setHitRecord(this, this->t_hit, this->normal_v);
    return true;
};
void Triangle::info() {

};
