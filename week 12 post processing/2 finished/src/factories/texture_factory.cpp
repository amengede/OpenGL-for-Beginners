#include "texture_factory.h"
#include "../stb_image.h"

void TextureFactory::start_texture_array(int layers) {

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, 1024, 1024, layers);
}

void TextureFactory::load_into_array(const char* filename, int layer) {
    
    //load image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(
        filename, &width, &height, &channels, STBI_rgb_alpha);

    //upload to GPU
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 
        0, 0, layer, 
        width, height, 1,
        GL_RGBA, GL_UNSIGNED_BYTE, data);

    //free data
	stbi_image_free(data);
}

unsigned int TextureFactory::finalize_texture_array() {

    //Configure sampler
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

    return texture;
}

unsigned int TextureFactory::build_cubemap(std::vector<const char*> filenames) {

    //make texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, 1024, 1024);

    //load images
    stbi_set_flip_vertically_on_load(true);
    for (int i = 0; i < 6; ++i) {
        int width, height, channels;
	    unsigned char* data = stbi_load(filenames[i], 
            &width, &height, &channels, STBI_rgb_alpha);

        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
  	        0, 0, width, height,
  	        GL_RGBA, GL_UNSIGNED_BYTE, data);

        //free data
	    stbi_image_free(data);
    }

    return texture;
}

unsigned int TextureFactory::build_colorbuffer(GLFWwindow* window) {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    unsigned int colorbuffer;
    glGenTextures(1, &colorbuffer);
    glBindTexture(GL_TEXTURE_2D, colorbuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);

    return colorbuffer;
}

unsigned int TextureFactory::build_depthbuffer(GLFWwindow* window) {

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    unsigned int depthbuffer;
    glGenRenderbuffers(1, &depthbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    return depthbuffer;
}

unsigned int TextureFactory::build_framebuffer(
    unsigned int colorbuffer, unsigned int depthbuffer) {

    unsigned int framebuffer;

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_2D, colorbuffer, 0);
    
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
        GL_RENDERBUFFER, depthbuffer);

    return framebuffer;
}