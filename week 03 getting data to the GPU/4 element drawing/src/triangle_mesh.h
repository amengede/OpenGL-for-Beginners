#pragma once
#include <vector>

class TriangleMesh {
public:
TriangleMesh();
void draw();
~TriangleMesh();

private:
unsigned int EBO, VAO, element_count;
std::vector<unsigned int> VBOs;
};
