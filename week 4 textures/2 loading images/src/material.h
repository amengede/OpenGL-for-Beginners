#include "config.h"

class Material {
public:
    Material(char* filename);
    ~Material();
    void use();
private:
    unsigned int texture;
};