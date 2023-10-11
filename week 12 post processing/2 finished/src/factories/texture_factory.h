#pragma once
#include "../config.h"

class TextureFactory {

public:

    void start_texture_array(int layers);

    void load_into_array(const char* filename, int layer);
    
    unsigned int finalize_texture_array();

    unsigned int build_cubemap(std::vector<const char*> filenames);

    unsigned int build_colorbuffer(GLFWwindow* window);

    unsigned int build_depthbuffer(GLFWwindow* window);

    unsigned int build_framebuffer(unsigned int colorbuffer, unsigned int depthbuffer);

private:

    unsigned int rows, cols;

    unsigned int texture;

};