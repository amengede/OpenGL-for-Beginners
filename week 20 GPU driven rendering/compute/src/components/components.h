#pragma once
#include "../config.h"

struct GameObject {
    float pos_and_scale[4];
    float euler_and_zero[4];
    float angular_velocity[3];
    uint32_t objectType;
};