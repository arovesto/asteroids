#include "Vec.h"
#include <random>

#include "Engine.h"

Vec random_vec(float intencity) {
    return Vec{float(rand() / (RAND_MAX + 1.) * intencity - intencity / 2), float(rand() / (RAND_MAX + 1.) * intencity - intencity / 2)};
};

Vec random_in_map() {
    return Vec{float(rand() / (RAND_MAX + 1.)) * SCREEN_WIDTH, float(rand() / (RAND_MAX + 1.)) * SCREEN_HEIGHT};
};