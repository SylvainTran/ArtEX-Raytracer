#pragma once
#include <stdio.h>
#include <Eigen/Core>

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
};
