#pragma once
#include <vector>

#include "Vec.h"


class Figure {
public:
    // Figure should be created from array of points (last and first NOT connected) and center point (if rotation supposed)
    Figure(const std::vector<Vec>& pts) : points(pts) {};
    void draw(Vec& displacement, float angle, float factor = 1);
    std::vector<Vec>& get_points() { return points; };
private:
    // lines drawn between this points
    std::vector<Vec> points;
};

