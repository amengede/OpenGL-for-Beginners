#include "config.h"

class Material {
public:
    Material(const char* filename);
    ~Material();
    void use();
private:
    unsigned int texture;
};