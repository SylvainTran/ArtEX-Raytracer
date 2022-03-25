#pragma once
#include <iostream>
#include <Eigen/Core>
using Eigen::Vector3f;
using Eigen::Vector4f;

template <class T>
struct Point3 {
    T x;
    T y;
    T z;

    Point3() : x(0), y(0), z(0) {};
    Point3(T x, T y, T z) : x(x), y(y), z(0) {};
    Point3(Point3& p) {
        this->x = p.x;
        this->y = p.y;
        this->z = p.z;
    };
    Point3(Vector4f p) {
        this->x = p(0);
        this->y = p(1);
        this->z = p(2);
    };
    Point3& operator=(const Point3& p) {
        this->x = p.x;
        this->y = p.y;
        this->z = p.z;
        return *this;
    };
    Vector3f operator-(const Point3<T>& p) const {
        return Vector3f(x - p.x, y - p.y, z - p.z);
    }
    Point3& operator/(T value) {
        this->x /= value;
        this->y /= value;
        this->z /= value;
        return *this;
    };
    friend inline std::ostream& operator<<(std::ostream& out, const Point3<float>& p) {
        out << p.x;
        out << ", ";
        out << p.y;
        out << ", ";
        out << p.z;
        return out;
    };
};
typedef Point3<float> Point3f;
typedef Point3<int>   Point3i;
