#include "config.h"

class Cube {

public:
    Cube(glm::vec3 position, glm::vec3 size);

    ~Cube();

    void update(float dt);

    void draw(unsigned int shader);
private:
    glm::vec3 position, eulers;
    unsigned int VAO, VBO, texture;

    void make_texture();
    void make_mesh(float l, float w, float h);
};