#pragma once
#include <stdio.h>
#include <iostream>
#include <Eigen/Core>
using Eigen::Vector3f;
using std::cout;
using std::endl;

class Utility {
    
public:
    static float degToRad(float deg) {
        return deg * (M_PI/180);
    };
    static float radToDeg(float rad) {
        return rad * (180/M_PI);
    };
    static float max(float a, float b) {
      return a>b?a:b;
    };
    static float min(float a, float b) {
      return a<b?a:b;
    };
    static float clamp(float val, float min, float max) {
      if(val < min) {
        val = min;
      }
      if(val > max) {
        val = max;
      }
      return val;
    };
    static float squaredDistance(Eigen::Vector3f p1, Eigen::Vector3f p2) {
        return (p1(0)-p2(0))*(p1(0)-p2(0))
                + ((p1(1)-p2(1))*(p1(1)-p2(1)))
                + ((p1(2)-p2(2))*(p1(2)-p2(2)));
    };
    static Eigen::Vector3f clampVectorXf(Eigen::Vector3f value, float min, float max) {
      Eigen::Vector3f clamped;
      if(value(0) < min) {
        clamped(0) = min;
      } else if(value(0) > max) {
        clamped(0) = max;
      } else {
        clamped(0) = value(0);
      }

      if(value(1) < min) {
        clamped(1) = min;
      }
      if(value(1) > max) {
        clamped(1) = max;
      } else {
        clamped(1) = value(1);
      }

      if(value(2) < min) {
        clamped(2) = min;
      }
      if(value(2) > max) {
        clamped(2) = max;
      } else {
        clamped(2) = value(2);
      }
      return clamped;
    };
    // from PBR book
    static float nextFloatUp(float v) {
        if (std::isinf(v) && v > 0.)
            return v;
        if (v == -0.f)
            v = 0.f;

        uint32_t ui = floatToBits(v);
        if (v >= 0) ++ui;
        else        --ui;
        return bitsToFloat(ui);
    };
    static float nextFloatDown(float v) {
        if (std::isinf(v) && v > 0.) return v;
        if (v == -0.f) v = 0.f;
        uint32_t ui = floatToBits(v);
        if (v >= 0) --ui;
        else ++ui;
        return bitsToFloat(ui);
    };
    static uint32_t floatToBits(float f) {
        uint32_t ui;
        memcpy(&ui, &f, sizeof(float));
        return ui;
    };
    static float bitsToFloat(uint32_t ui) {
        float f;
        memcpy(&f, &ui, sizeof(uint32_t));
        return f;
    };
    static Vector3f getOffsetRayOrigin(const Vector3f &x, float bias,
                                       const Vector3f &n, const Vector3f &dir) {
        Vector3f offset = bias * Vector3f(n);
        if (dir.dot(n) < 0 && x(2) < 0 && dir(2) < 0) {
            // Push in front by being positive
            offset = -offset;
        }
        Vector3f offsetOrigin = x + offset;
        for (int i = 0; i < 3; ++i) {
            if (offset[i] > 0) offsetOrigin[i] = nextFloatUp(offsetOrigin[i]);
            else if (offset[i] < 0) offsetOrigin[i] = nextFloatDown(offsetOrigin[i]);
        }
        return offsetOrigin;
    };
    // Disk or square
    static Vector3f sampleOverUnitArea(Vector3f intersected_point, Vector3f n, Vector3f& rand_point, Vector3f& rand_dir_vec) {

        srand((unsigned)time( NULL ));
        srand48((unsigned)time( NULL ));

        bool outside = true;
        while(outside) {
            // Sampling between [-1,1]
            // -----------------------
            rand_point(0) = (2 * drand48()) - 1;
            rand_point(1) = (2 * drand48()) - 1;
            rand_point(2) = (2 * drand48()) - 1;

            rand_dir_vec(0) = (2 * drand48()) - 1;
            rand_dir_vec(1) = (2 * drand48()) - 1;
            rand_dir_vec(2) = (2 * drand48()) - 1;

            outside = std::abs(rand_point.norm()) > 1.0f;
        }

        if(rand_point.dot(n) < 0) {
            rand_point *= -1;
        }
        Vector3f frame_left = rand_point.cross(n);
        Vector3f frame_z = n.cross(frame_left);
        Eigen::Matrix3f matrix_R = Utility::transformPointToLocalObject(frame_left, n, frame_z);
        rand_dir_vec = (matrix_R.inverse() * rand_point) + intersected_point;

        // cout << " rand dir vec world coord : " << rand_dir_vec << endl;
        return rand_dir_vec;
    };
    static Eigen::Matrix3f transformPointToLocalObject(Vector3f frame_left, Vector3f x_normal, Vector3f frame_z) {
        Eigen::Matrix3f matrix_R {
                {frame_left(0),frame_left(1),frame_left(2)},
                {x_normal(0),x_normal(1),x_normal(2)},
                {frame_z(0),frame_z(1),frame_z(2)}
        };
        return matrix_R;
    };
};
