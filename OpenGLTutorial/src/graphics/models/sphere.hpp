#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../objects/model.h"

class Sphere : public Model {
public:
    Sphere(unsigned int maxNoInstances)
        : Model("sphere", maxNoInstances, DYNAMIC) 
    {
        
    }

    void init() {
        loadModel("assets/models/sphere/scene.gltf");
    }
};

#endif