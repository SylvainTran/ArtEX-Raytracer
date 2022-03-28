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

    BVHNode(std::vector<Surface*>& list) {
        make(list);
    };

    BVHNode(Surface* s) : bbox(s->bbox), left(nullptr), right(nullptr) { // leaf node

    };

    BVHNode(BoundingBox* bbox, BVHNode* left, BVHNode*right) : bbox(bbox), left(left), right(right) { // node with subtree

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

    struct {
        bool operator()(Surface* a, Surface* b) const { return a->bbox->x_min < b->bbox->x_min && a->bbox->x_max < b->bbox->x_max; }
    } sortLesserBySurfaceXAxis;

    BVHNode& make(std::vector<Surface*>& list) {
        int sz = list.size();
        if(sz == 1) {
            cout << " sz/1 " << endl;
            left = new BVHNode(list[0]);
            right = nullptr;
            bbox = left->bbox;
            cout << " case sz == 1" << endl;
            return *this;
        } else if(sz == 2) {
            cout << " case sz == 2" << endl;
            left = new BVHNode(list[0]);
            right = new BVHNode(list[1]);
            // Combine
            cout << " reading list[0] and [1]'s bboxes" << endl;
            cout << *list[0]->bbox << *list[1]->bbox << endl;
            cout << "COMBINE " << endl;

            bbox = this->combine(list[0]->bbox, list[1]->bbox);
            cout << " / COMBINE " << endl;
            return *this;
        } else if(sz > 2) {

            cout << " sz > 2 ?//" << endl;
            for(Surface* s : list) {
                if(s != nullptr && s->bbox != nullptr)
                    cout << *s->bbox << endl;
                else
                    cout << " SHITT " << endl;
            }

            cout << " c/ " << endl;
            if(*list.begin() != *list.end() && !list.empty()) {
                if(list.size() == 2)
                    std::sort(list.begin(), list.end(), sortLesserBySurfaceXAxis);
            } else {
                cout << "list size == 0" << list.size() << endl;
                return *this;
            }

            cout << "Sorted by x-axis" << endl;
            for(Surface* s : list) {
                if(s != nullptr && s->bbox != nullptr)
                    cout << *s->bbox << endl;
            }

            cout << " hrere" << endl;

            Vector3f midpoint = list[list.size() / 2]->centre;//this->midpoint(list);
            int midpoint_index = list.size() / 2;

            std::vector<Surface*> left_midpoint, right_midpoint;

            auto start = list.begin();
            auto midpoint_iter = list.begin() + midpoint_index + 1;
            auto listEnd = list.end();

            if(start != listEnd) {
                vector<Surface*> leftSublist(midpoint_index + 1);
                copy(start, midpoint_iter, leftSublist.begin());

                vector<Surface*> rightSublist(list.size() - midpoint_index + 1);
                copy(midpoint_iter, listEnd, rightSublist.begin());

                if(leftSublist.size() >= 1)  left = new BVHNode(leftSublist);
                if(rightSublist.size() >= 1) right = new BVHNode(rightSublist);

                cout <<" // COMBINING " << endl;
                if(left->bbox != nullptr && right->bbox != nullptr)
                    bbox = combine(left->bbox, right->bbox);
                cout << " / combining " << endl;
            }

//
//            cout << "Left sublist" << endl;
//            for(Surface* s : leftSublist) {
//                cout << *s->bbox << endl;
//            }
//            cout << "Right sublist" << endl;
//            for(Surface* s : rightSublist) {
//                cout << *s->bbox << endl;
//            }
            return *this;
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
        cout << " in combine funct " << endl;
        float min_x_combined, min_y_combined, max_x_combined, max_y_combined;
        min_x_combined = std::min(b1->x_min, b2->x_min);
        min_y_combined = std::min(b1->y_min, b2->y_min);
        max_x_combined = std::max(b1->x_max, b2->x_max);
        max_y_combined = std::max(b1->y_max, b2->y_max);
        cout << " / in combine funct " << endl;

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