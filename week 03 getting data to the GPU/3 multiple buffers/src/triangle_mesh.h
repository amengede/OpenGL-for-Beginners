#pragma once
#include <vector>

class TriangleMesh {
public:
TriangleMesh();
void draw();
~TriangleMesh();

private:
unsigned int VAO, vertex_count;
std::vector<unsigned int> VBOs;
};
