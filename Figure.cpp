#include <math.h>
#include <iostream>

#include "Figure.h"
#include "Draw.h"

void Figure::draw(Vec& displacement, float angle, float factor) {
    if (displacement.x > SCREEN_WIDTH ) {
        displacement.x = 0;
    }
    if (displacement.x < 0 ) {
        displacement.x = SCREEN_WIDTH;
    }
    if (displacement.y > SCREEN_HEIGHT) {
        displacement.y = 0;
    }
    if (displacement.y < 0) {
        displacement.y = SCREEN_HEIGHT;
    }
    for (size_t i = 1; i < points.size(); i++) {
        draw_line(points[i - 1].rotate(angle).mul(factor).add(displacement), points[i].rotate(angle).mul(factor).add(displacement));
    }
};