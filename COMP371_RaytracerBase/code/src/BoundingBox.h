#pragma once
#include <Eigen/Core>
#include "Surface.h"
#include "Rectangle.h"
using Eigen::Vector3f;
using std::cout;
using std::endl;

struct BoundingBox : public Surface {
    std::string surfaceType;
    Surface* surfaceBound;
    float x_min, x_max, y_min, y_max, z_min, z_max;

    BoundingBox(float x_min, float x_max, float y_min, float y_max, float z_min, float z_max) {
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
        x_min = s->triangle1->p2.x() + 0.1f;
        x_max = s->triangle2->p3.x() + 0.1f;
        y_min = s->triangle1->p2.y() + 0.1f;
        y_max = s->triangle2->p2.y() + 0.1f;
        z_min = -1*(s->triangle1->p2.z()) + 0.1f;
        z_max = s->triangle2->p2.z() + 0.1f;
        cout << x_min << endl;
        cout << x_max << endl;
        cout << y_min << endl;
        cout << y_max << endl;
        cout << z_min << endl;
        cout << z_max << endl;
    };

    bool hit(Ray& r, float t0, float t1, HitRecord& hitReturn) {
//        float t_x_min, t_x_max, t_y_min, t_y_max, t_z_min, t_z_max;
//        float e_x, e_y, e_z;
//        e_x = r.o.x();
//        e_y = r.o.y();
//        e_z = r.o.z();
//
//        float inverseDirX = 1/r.d.x();
//        if(r.d.x() >= 0) {
//            t_x_min = inverseDirX*(x_min - e_x);
//            t_x_max = inverseDirX*(x_max - e_x);
//        } else {
//            t_x_min = inverseDirX*(x_max - e_x);
//            t_x_max = inverseDirX*(x_min - e_x);
//        }
//
//        float inverseDirY = 1/r.d.y();
//        if(r.d.y() >= 0) {
//            t_y_min = inverseDirY*(y_min - e_y);
//            t_y_max = inverseDirY*(y_max - e_y);
//        } else {
//            t_y_min = inverseDirY*(y_max - e_y);
//            t_y_max = inverseDirY*(y_min - e_y);
//        }
//        float inverseDirZ = 1/r.d.z();
//        if(r.d.z() >= 0) {
//            t_z_min = inverseDirZ*(z_min - e_z);
//            t_z_max = inverseDirZ*(z_max - e_z);
//        } else {
//            t_z_min = inverseDirZ*(z_max - e_z);
//            t_z_max = inverseDirZ*(z_min - e_z);
//        }
//        bool hitResult =  (t_x_min >= x_min && t_x_max <= x_max &&
//                t_y_min >= y_min && t_y_max <= y_max &&
//                t_z_min >= z_min && t_z_max <= z_max);

       // if(hitResult) {
//            cout << "t_x_min" << t_x_min << endl;
//            cout << "t_y_min" << t_y_min << endl;
//            cout << "t_z_min" << t_z_min << endl;
//            cout<<"r.d()x "<<r.d.x()<<endl;
//            cout<<"r.d()y "<<r.d.y()<<endl;
//            cout<<"r.d()z "<<r.d.z()<<endl;
            return this->surfaceBound->hit(r, t0, t1, hitReturn);
        //}
        //return hitResult;
    };
};