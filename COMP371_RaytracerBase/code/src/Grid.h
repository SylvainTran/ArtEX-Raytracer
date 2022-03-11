#pragma once
#include "BoundingBox.h"
#include <vector>

//one cell
struct Grid : public Surface {
    std::vector<BoundingBox*> bboxesInCell;
};

