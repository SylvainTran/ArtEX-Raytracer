#pragma once
#include <vector>
#include <Eigen/Core>
#include "Surface.h"
#include "Rectangle.h"
#include "Sphere.h"
using Eigen::Vector3f;
using std::cout;
using std::endl;

struct BoundingBox : public Surface {
    std::string surfaceType;
    Surface* surfaceBound;
    float x_min, x_max, y_min, y_max, z_min, z_max;

    BoundingBox() {
        surfaceType = "Empty Bounding Box";
        surfaceBound = nullptr;
    };

    BoundingBox(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) {
        surfaceType = "world";
        surfaceBound = nullptr;
        this->x_min = x_min;
        this->x_max = x_max;
        this->y_min = y_min;
        this->y_max = y_max;
        this->z_min = z_min;
        this->z_max = z_max;
    };
    /**
     * The order of contruction is:
     *Triangle* triangle1 = new Triangle(P1,P2,P3);
      Triangle* triangle2 = new Triangle(P1,P3,P4);
     * @param s
     */
    BoundingBox(Rectangle* s) {
        surfaceType = "rectangle";
        surfaceBound = s;
//        cout << *this << endl;
//        exit(0);
        x_min = s->triangle1->p1.x();
        x_max = s->triangle2->p2.x();
        y_min = s->triangle1->p1.y();
        y_max = s->triangle2->p2.y();

        z_min = s->triangle1->p1.z();
        z_max = s->triangle1->p3.z();

//        cout << "x_min: " << x_min << endl;
//        cout << "x_max: " << x_max << endl;
//        cout << "y_min: " << y_min << endl;
//        cout << "y_max: " << y_max << endl;
//        cout << "z_min: " << z_min << endl;
//        cout << "z_max: " << z_max << endl;
    };

    BoundingBox(Sphere* s) {
        if(s->centre.x() > 0) {
            x_min = s->centre.x() - s->radius;
            x_max = s->centre.x() + s->radius;
        } else {
            x_min = s->centre.x() + s->radius;
            x_max = s->centre.x() - s->radius;
        }
        if(s->centre.y() > 0) {
            y_min = s->centre.y() - s->radius;
            y_max = s->centre.y() + s->radius;
        } else {
            y_min = s->centre.x() + s->radius;
            y_min = s->centre.x() - s->radius;
        }
        surfaceBound = s;
        surfaceType = "Sphere";
    };

    BoundingBox(Surface* s) {
        cout << "No " << endl;
        surfaceBound = s;
        surfaceType = "Surface base";
    };

    BoundingBox(BoundingBox& bbox) {
        surfaceType = "node";
        surfaceBound = nullptr;
        this->x_min = bbox.x_min;
        this->x_max = bbox.x_max;
        this->y_min = bbox.y_min;
        this->y_max = bbox.y_max;
        this->z_min = bbox.z_min;
        this->z_max = bbox.z_max;
    };

    BoundingBox& operator=(BoundingBox& bbox) {
        surfaceType = "node";
        surfaceBound = nullptr;
        this->x_min = bbox.x_min;
        this->x_max = bbox.x_max;
        this->y_min = bbox.y_min;
        this->y_max = bbox.y_max;
        this->z_min = bbox.z_min;
        this->z_max = bbox.z_max;

        return *this;
    };

    bool hit(Ray& r, float t0, float t1, HitRecord& hitReturn) { // this function already yields 2x acceleration

        float t_x_min, t_x_max, t_y_min, t_y_max, t_z_min, t_z_max;
        float e_x, e_y, e_z;
        e_x = r.o.x();
        e_y = r.o.y();
        e_z = r.o.z();

        float inverseDirX = 1/r.d.x(), inverseDirY = 1/r.d.y(), inverseDirZ = 1/r.d.z();

        if (inverseDirX >= 0) {
            t_x_min = inverseDirX * (x_min - e_x);
            t_x_max = inverseDirX * (x_max - e_x);
        } else {
            t_x_min = inverseDirX * (x_max - e_x);
            t_x_max = inverseDirX * (x_min - e_x);
        }

        if (inverseDirY >= 0) { // confusing logic for y -- the first bound hit is the reverse here (see signs in points for scene 2 vs. cornell)
            if (y_min >= 0 && y_max >= 0) {
                t_y_min = inverseDirY * (y_min - e_y);
                t_y_max = inverseDirY * (y_max - e_y);
            } else {
                t_y_min = inverseDirY * (y_max - e_y);
                t_y_max = inverseDirY * (y_min - e_y);
            }
        } else {
            if (y_min >= 0 && y_max >= 0) {
                t_y_min = inverseDirY * (y_max - e_y);
                t_y_max = inverseDirY * (y_min - e_y);
            } else {
                t_y_min = inverseDirY * (y_min - e_y);
                t_y_max = inverseDirY * (y_max - e_y);
            }
        }

        if (t_x_max < t_y_min || t_x_min > t_y_max) {
            return false;
        }

// FIXME: Z and closest hit problem / backface rendering

//        if (inverseDirZ >= 0) { // z lookat = 1
//            if(z_min >= 0 && z_max >= 0) { // z's all +
//                t_z_min = inverseDirZ * (z_min - e_z);
//                t_z_max = inverseDirZ * (z_max - e_z);
//            } else { // z's all -
//                t_z_min = inverseDirZ * (z_max - e_z); // max is smallest neg value
//                t_z_max = inverseDirZ * (z_min - e_z);
//            }
//        } else { // z lookat = =1
//            if (z_min > 0 && z_max > 0) {
//                t_z_min = inverseDirZ * (z_max - e_z);
//                t_z_max = inverseDirZ * (z_min - e_z);
//            } else if (z_min < 0 && z_max < 0) {
//                t_z_min = inverseDirZ * (z_min - e_z);
//                t_z_max = inverseDirZ * (z_max - e_z);
//            } else if (z_min < 0 && z_max > 0) {
//                // should be here mostly
//                t_z_min = inverseDirZ * (z_max - e_z);
//                t_z_max = inverseDirZ * (z_min - e_z);
//            } else if (z_min > 0 && z_max < 0) {
//                t_z_min = inverseDirZ * (z_max - e_z);
//                t_z_max = inverseDirZ * (z_min - e_z);
//            } else { // z's can be both 0
//                t_z_min = inverseDirZ * (z_min - e_z);
//                t_z_max = inverseDirZ * (z_max - e_z);
//            }
//        }
//
//        if (t_z_min > z_max || t_z_max < z_min) {
////            cout << " t_z_min : " << t_z_min << endl;
////            cout << " t_z_max : " << t_z_max << endl;
//            return false;
//        }

//        cout << " Hit box true" << endl;
        return true;
    };

    friend inline std::ostream& operator<<(std::ostream& os, BoundingBox& bbox) {
        os << " " << bbox.surfaceType << "x min= " << bbox.x_min << " x max= " << bbox.x_max << " y min= " << bbox.y_min << " y max= " << bbox.y_max << endl;
        return os;
    };
};
/**
* //        cout << " inside !" << endl;
//        cout << " r.o : " << r.o << endl; // NDC
//        cout << " r.d: " << r.d << endl;
//        cout << " r.d.x() : " << r.d.x() << endl;
//
//        cout << " t_x_min : " << t_x_min << endl; // World coordinates
//        cout << " t_x_max : " << t_x_max << endl;
//        cout << " t_y_min : " << t_y_min << endl;
//        cout << " t_y_max : " << t_y_max << endl;
*/