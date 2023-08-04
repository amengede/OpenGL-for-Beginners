#include "cube.h"
#include "stb_image.h"

Cube::Cube(glm::vec3 position, glm::vec3 size) {
    this->position = position;
    this->eulers = glm::vec3(0.0f, 0.0f, 0.0f);

    make_texture();
    make_mesh(size.x, size.y, size.z);
}

void Cube::make_texture() {
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(
        "../img/paper.jpg", &width, &height, &channels, STBI_rgb_alpha);

	//make the texture
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	
    //load data
    glTexImage2D(GL_TEXTURE_2D, 
        0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);

    //free data
	stbi_image_free(data);

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Cube::make_mesh(float l, float w, float h) {

    std::vector<float> vertices = {
         l,  w, -h, 1.0f, 1.0f,
         l, -w, -h, 1.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l,  w, -h, 0.0f, 1.0f,
         l,  w, -h, 1.0f, 1.0f,

        -l, -w,  h, 0.0f, 0.0f,
         l, -w,  h, 1.0f, 0.0f,
         l,  w,  h, 1.0f, 1.0f,
         l,  w,  h, 1.0f, 1.0f,
        -l,  w,  h, 0.0f, 1.0f,
        -l, -w,  h, 0.0f, 0.0f,

        -l,  w,  h, 1.0f, 1.0f,
        -l,  w, -h, 1.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l, -w, -h, 0.0f, 0.0f,
        -l, -w,  h, 0.0f, 1.0f,
        -l,  w,  h, 1.0f, 1.0f,

         l, -w, -h, 0.0f, 0.0f,
         l,  w, -h, 1.0f, 0.0f,
         l,  w,  h, 1.0f, 1.0f,
         l,  w,  h, 1.0f, 1.0f,
         l, -w,  h, 0.0f, 1.0f,
         l, -w, -h, 0.0f, 0.0f,

        -l, -w, -h, 0.0f, 0.0f,
         l, -w, -h, 1.0f, 0.0f,
         l, -w,  h, 1.0f, 1.0f,  
         l, -w,  h, 1.0f, 1.0f,
        -l, -w,  h, 0.0f, 1.0f,
        -l, -w, -h, 0.0f, 0.0f,

         l,  w,  h, 1.0f, 1.0f,
         l,  w, -h, 1.0f, 0.0f,
        -l,  w, -h, 0.0f, 0.0f, 
        -l,  w, -h, 0.0f, 0.0f,
        -l,  w,  h, 0.0f, 1.0f,
         l,  w,  h, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
        vertices.data(), GL_STATIC_DRAW);
    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (void*)0);
    glEnableVertexAttribArray(0);
    //texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (void*)12);
    glEnableVertexAttribArray(1);
}

void Cube::update(float dt) {

    eulers.z += 10.0f * dt;

    if (eulers.z > 360) {
        eulers.z -= 360;
    }
}

void Cube::draw(unsigned int shader) {

    glUseProgram(shader);

    //model transform
    glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::rotate(
        model, glm::radians(eulers.z), 
        { 0.0f, 0.0f, 1.0f });
	glUniformMatrix4fv(
        glGetUniformLocation(shader, "model"), 
        1, GL_FALSE, value_ptr(model));
    
    //material
    glBindTexture(GL_TEXTURE_2D, texture);
    //mesh
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Cube::~Cube() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &texture);
}