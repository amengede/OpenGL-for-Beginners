#include "material.h"
#include "stb_image.h"

Material::Material(char* filename) {
    int width, height, channels;
	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

	//make the texture
	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	
    //load data
    glTexImage2D(GL_TEXTURE_2D, 
        0, GL_RGBA, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);

    //free data
	stbi_image_free(data);
}

Material::~Material() {
    glDeleteTextures(1, &texture);
}

void Material::use() {
    glBindTexture(GL_TEXTURE_2D, texture);
}