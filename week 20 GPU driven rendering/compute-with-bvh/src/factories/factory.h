#pragma once
#include "../config.h"
#include "../components/components.h"

class Factory {

public:
    Factory();

    GameObject make_object();

    uint32_t objectCounts[objectTypeCount];
private:
    uint32_t objectsMade = 0;

};