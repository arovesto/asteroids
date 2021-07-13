#pragma once
#include <math.h>

struct Vec {
    float x;
    float y;
    Vec rotate(float angle) {
        float sine = sin(angle);
        float cosine = cos(angle);

        return Vec{x * cosine - y * sine, x * sine + y * cosine};
    }
    Vec add(const Vec& other) {
        return Vec{x + other.x, y + other.y};
    }
    Vec mul(const float c) {
        return Vec{x*c,y*c};
    }
    Vec sub(const Vec& other) {
        return Vec{x - other.x, y - other.y};
    }
    Vec norm() {
        return mul(1/sqrt(abs_square()));
    }
    float angle() {
        Vec v = norm();
        return atan2(v.y, v.x);
    }
    float abs_square() {
        return x * x + y * y;
    }
    Vec clamp(float bound) {
        float new_x = x;
        if (new_x > bound) {
            new_x = bound;
        }
        if (new_x < -bound) {
            new_x = -bound;
        }
        float new_y = y;
        if (new_y > bound) {
            new_y = bound;
        }
        if (new_y < -bound) {
            new_y = -bound;
        }
        return Vec{new_x, new_y};
    }
};

Vec random_vec(float intencity);

Vec random_in_map();