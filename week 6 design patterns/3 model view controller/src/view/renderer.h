#pragma once
#include "../config.h"
#include "shader.h"
#include "cube_mesh.h"
#include "material.h"
#include "../model/scene.h"

class Renderer {
public:
    Renderer(GLFWwindow* window);
    void render(Scene* scene);
    ~Renderer();
private:
    void set_up_opengl(GLFWwindow* window);
    void make_assets();

    CubeMesh* cubeMesh;
    Material* cubeMaterial;
    GLFWwindow* window;
    unsigned int shader, modelLocation, viewLocation;
};