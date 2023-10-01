#pragma once
#include "../config.h"

class CubeMesh {

public:
    CubeMesh(glm::vec3 size);

    ~CubeMesh();

    void draw();
private:
    unsigned int VAO, VBO;
    
    void make_mesh(float l, float w, float h);
};