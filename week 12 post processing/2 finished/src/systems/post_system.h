#pragma once
#include "../config.h"

class PostSystem {
public:

    PostSystem(unsigned int shader, 
        unsigned int colorbuffer);
    
    void update();
    
private:

    unsigned int shader, colorbuffer;

};