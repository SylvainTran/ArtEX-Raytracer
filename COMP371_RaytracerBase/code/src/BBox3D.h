#pragma once
#include "Point3.h"

template<class T, class U> struct BBox3D {
    Point3<T> minBound; // bottomLeft front
    Point3<T> maxBound; // top right back
    Point3<T> bottomRight, topLeft;

    BBox3D() {
        // From pbr book. To make sure that Unions are OK by setting the worst
        // possible bounds at first.
        float minNum = std::numeric_limits<T>::lowest();
        float maxNum = std::numeric_limits<T>::max();
        Point3<T> p1(maxNum, maxNum, maxNum);
        Point3<T> p2(minNum, minNum, minNum);
        this->minBound = p1;
        this->maxBound = p2;
    };
    BBox3D(BBox3D& b) {
        this->minBound = b.minBound;
        this->maxBound = b.maxBound;
    };
    BBox3D<float,float>& operator=(BBox3D<float,float>& b) {
        this->minBound = b.minBound;
        this->maxBound = b.maxBound;
        return *this;
    };
    BBox3D(Point3<T>& point) : minBound(point), maxBound(point) {};
    BBox3D(const Point3<T>& p1, const Point3<T>& p2)
            : minBound(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)),
              maxBound(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)) {};
    // All corners;
    BBox3D(const Point3<T>& p1, const Point3<T>& p2, const Point3<T>& p3, const Point3<T>& p4)
            : minBound(std::min(p1.x, p2.x), std::min(p1.y, p2.y), std::min(p1.z, p2.z)),
              maxBound(std::max(p1.x, p2.x), std::max(p1.y, p2.y), std::max(p1.z, p2.z)) {
        this->bottomRight = p3;
        this->topLeft = p4;
    };

    // Convenience -> i = 0 = minBound, 1 = maxBound
    const Point3<T>& operator[](int i) const {
        if(i == 0){
            return minBound;
        } else if(i == 1) {
            return maxBound;
        }
    };
    // Non-const version
    Point3<T>& operator[](int i) {
        if(i == 0){
            return minBound;
        } else if(i == 1) {
            return maxBound;
        }
    };

    friend inline std::ostream& operator<<(std::ostream& out, const BBox3D& b) {
        out << "min bound: ";
        out << b.minBound;
        out << ", max bound: ";
        out << b.maxBound;
        return out;
    };
    // Creates a box that contains both the box and the point
    BBox3D<T, U>& getUnionBBoxPoint(const BBox3D<T, U>&b , const Point3<T>& p) {
        Point3<T> p1(std::min(b.minBound.x, p.x),
                     std::min(b.minBound.y, p.y),
                     std::min(b.minBound.z, p.z));
        Point3<T> p2(std::max(b.maxBound.x, p.x),
                     std::max(b.maxBound.y, p.y),
                     std::max(b.maxBound.z, p.z));
        BBox3D<T, U> ret(p1, p2);
        return ret;
    };
    // Creates a box that encompasses both boxes
    BBox3D<T, U>& getUnionBBox3D(const BBox3D<T, U>& first, const BBox3D<T, U>& second) const {
        Point3<T> p1(std::min(first.minBound.x, second.minBound.x),
                     std::min(first.minBound.y, second.minBound.y),
                     std::min(first.minBound.z, second.minBound.z));
        Point3<T> p2(std::max(first.maxBound.x, second.maxBound.x),
                     std::max(first.maxBound.y, second.maxBound.y),
                     std::max(first.maxBound.z, second.maxBound.z));
        BBox3D<T, U> ret(p1, p2);
        return ret;
    };
    // Get a new box intersecting both boxes
    BBox3D<T, U>& getIntersection(const BBox3D<T, U> &b1, const BBox3D<T, U> &b2) {
        return BBox3D<T, U>(Point3<T>(std::max(b1.minBound.x, b2.minBound.x),
                                      std::max(b1.minBound.y, b2.minBound.y),
                                      std::max(b1.minBound.z, b2.minBound.z)),
                            Point3<T>(std::min(b1.maxBound.x, b2.maxBound.x),
                                      std::min(b1.maxBound.y, b2.maxBound.y),
                                      std::min(b1.maxBound.z, b2.maxBound.z)));
    };
    // overlap test
    bool overlaps(const BBox3D<T, U> &b1, const BBox3D<T, U> &b2) {
        bool x_inside = (b1.maxBound.x >= b2.minBound.x) && (b1.minBound.x <= b2.maxBound.x);
        bool y_inside = (b1.maxBound.y >= b2.minBound.y) && (b1.minBound.y <= b2.maxBound.y);
        bool z_inside = (b1.maxBound.z >= b2.minBound.z) && (b1.minBound.z <= b2.maxBound.z);
        return (x_inside && y_inside && z_inside);
    };
    // Is the point inside test (point vs. box)
    bool inside(const Point3<T>& p, const BBox3D<T, U>& b) const {
        //std::cout << "inside test: " << std::endl;
        //std::cout << p;
        //std::cout << b;

        return (p.x >= b.minBound.x && p.x <= b.maxBound.x &&
                p.y >= b.minBound.y && p.y <= b.maxBound.y &&
                p.z >= b.minBound.z && p.z <= b.maxBound.z);
    };
//    BBox3D& expand(const BBox3D<T, U>& b, U delta) {
//        return BBox3D<T, U>(b.minBound - Vector3<T>(delta, delta, delta),
//                          b.maxBound + Vector3<T>(delta, delta, delta));
//    };
    // Get the center
    Vector3f getCenter() const {
        return 0.5f * (minBound + maxBound);
    };
    // Get the extents
    Vector3f getExtents() const {
        return 0.5f * (maxBound - minBound);
    };
    // Default 'contains' test with a single parameter
    bool contains(const BBox3D& aabb) const {
        return minBound.x <= aabb.minBound.x &&
               minBound.y <= aabb.minBound.y &&
               minBound.z <= aabb.minBound.z &&
               aabb.maxBound.x <= maxBound.x &&
               aabb.maxBound.y <= maxBound.y &&
               aabb.maxBound.z <= maxBound.z;
    };
    float Distance(const Point3<T>& p1, const Point3<T>& p2) {
        Vector3f v = p1-p2;
        return v.norm();
    }
    // Fast sphere
    void BoundingSphere(Point3<T> *center, float* radius) const {
        *center = (minBound + maxBound) / 2;
        *radius = inside(*center, *this) ? Distance(*center, maxBound) : 0;
    };
};

typedef BBox3D<float, float> BBox3Df;
typedef BBox3D<int, int>   BBox3Di;