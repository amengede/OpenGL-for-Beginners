#pragma once
#include "../config.h"
#include "material.h"

class Material {

public:
    Material(const char* filename);

    ~Material();

    void use();
private:
    unsigned int texture;
};