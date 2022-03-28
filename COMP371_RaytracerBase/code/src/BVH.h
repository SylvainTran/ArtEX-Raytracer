#pragma once
#include <vector>
#include "Surface.h"
#include "BoundingBox.h"

struct BVH {

    BVH(Surface* boundingBoxSurface) : node(boundingBoxSurface){};

    Surface* node;
    std::vector<Surface*> siblings;

    void insert(Surface* boundingBoxSurface) {
        this->siblings.push_back(boundingBoxSurface);
    };

    void buildTree(std::vector<Surface*> surfaces) {
        int len = surfaces.size();
        for(int i = 0 ; i < len; i++) {
            insert(surfaces[i]);
        }
    };
};

struct BVHNode : public Surface {
    BoundingBox* bbox;
    Surface* left;
    Surface* right;

    BVHNode(Surface* s) : bbox(s->bbox), left(nullptr), right(nullptr) { // leaf node
    };

    BVHNode(std::vector<Surface*> s) : left(nullptr), right(nullptr) {
    };

    BVHNode(BoundingBox* bbox, BVHNode* left, BVHNode*right) : bbox(bbox), left(left), right(right) { // node with subtree

    };

    void setBBox(BoundingBox* bbox) {
        this->bbox = bbox;
    };

    BVHNode(BVHNode& b) {
        bbox = b.bbox;
        left = b.left;
        right = b.right;
    };

    BVHNode& operator=(BVHNode& b) {
        bbox = b.bbox;
        left = b.left;
        right = b.right;
        return *this;
    };

    void make(std::vector<Surface*>& list) {
        int sz = list.size();
        if(sz == 1) {
            left = new BVHNode(list[0]);
            right = nullptr;
            bbox = left->bbox;
            cout << " case sz == 1" << endl;
            return;
        } else if(sz == 2) {
            cout << " case sz == 2" << endl;
            left = new BVHNode(list[0]); right = new BVHNode(list[1]);
            // Combine
            bbox = combine(left->bbox, right->bbox);
            return;
        } else {

            Vector3f midpoint = this->midpoint(list);
            cout << " CC" << endl;
            std::vector<Surface*> left_midpoint, right_midpoint;

            // Partition objects to the left and right of global midpoint
            partitionByMidpoint(midpoint, list, left_midpoint, right_midpoint);
            cout << " CC2" << endl;

            cout << " left " << endl;
//            for(Surface* s : left_midpoint) {
//                cout << *s->bbox << endl;
//            }
//
//            left = new BVHNode(nullptr, nullptr, nullptr);
//            left->make(left_midpoint);

//            cout << " right: " << right_midpoint.size() << endl;
//            for(Surface* s : right_midpoint) {
//                cout << *s->bbox << endl;
//            }

            right = new BVHNode(nullptr, nullptr, nullptr);

            cout <<" COMBINING " << endl;
            bbox = combine(left->bbox, right->bbox);
            return;
        }
    };
    void partitionByMidpoint(Vector3f midpoint, std::vector<Surface*>& list, std::vector<Surface*>& left, std::vector<Surface*>& right) {
        cout << "Partitioning! " << endl;

        for(Surface* s : list) {

            if(s->bbox->x_min < midpoint.x() && s->bbox->y_max < midpoint.y()) {
                left.push_back(s);
            }
            else {
                right.push_back(s);
            }
        }
        cout << "left sz: " << left.size() << ", right sz: " << right.size();
    };
    BoundingBox* combine(BoundingBox* b1, BoundingBox* b2) {
        float min_x_combined, min_y_combined, max_x_combined, max_y_combined;
        min_x_combined = std::min(b1->x_min, b2->x_min);
        min_y_combined = std::min(b1->y_min, b2->y_min);
        max_x_combined = std::max(b1->x_max, b2->x_max);
        max_y_combined = std::max(b1->y_max, b2->y_max);

        return new BoundingBox(min_x_combined, max_x_combined, min_y_combined, max_y_combined, -10000, 10000);
    };

    void findGlobalBox(std::vector<Surface*>& list, BoundingBox& globalBox) {

        float min_x_all = INFINITY, min_y_all = INFINITY, max_x_all = -INFINITY, max_y_all = -INFINITY;

        for(Surface* s : list) {
            if(s->bbox->x_min < min_x_all) min_x_all = s->bbox->x_min;
            if(s->bbox->y_min < min_y_all) min_y_all = s->bbox->y_min;

            if(s->bbox->x_max > max_x_all) max_x_all = s->bbox->x_max;
            if(s->bbox->y_max > max_y_all) max_y_all = s->bbox->y_max;
        }

        globalBox.x_min = min_x_all;
        globalBox.x_max = max_x_all;
        globalBox.y_min = min_y_all;
        globalBox.y_max = max_y_all;
    };

    Vector3f midpoint(std::vector<Surface*>& list) {

        BoundingBox* globalBox = new BoundingBox();
        findGlobalBox(list, *globalBox);
        cout << "globalBox x mid pt " << globalBox->x_min + globalBox->x_max / 2 << endl;
        cout << "globalBox y mid pt " << globalBox->y_min + globalBox->y_max / 2 << endl;
        // mid point = (x1+x2/2, y1+y2/2)
        return Vector3f(globalBox->x_min + globalBox->x_max / 2, globalBox->y_min + globalBox->y_max / 2, 0.f);
    };
};