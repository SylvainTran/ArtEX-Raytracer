#pragma once
#include <vector>
#include "Surface.h"

struct BVH {

    BVH(Surface* boundingBoxSurface) : node(boundingBoxSurface){

    };

    Surface* node;
    std::vector<Surface*> siblings;

    void insert(Surface* boundingBoxSurface) {
        this->siblings.push_back(boundingBoxSurface);
    };

    void createTree(std::vector<Surface*> surfaces) {
        int len = surfaces.size();
        for(int i = 0 ; i < len; i++) {
            insert(surfaces[i]);
        }
    };
};