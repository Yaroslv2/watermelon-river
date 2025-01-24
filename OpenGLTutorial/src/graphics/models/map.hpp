#ifndef MAP_HPP
#define MAP_HPP

#include "../objects/model.h"

class Map
	: public Model
{
public:
	Map(unsigned int maxNoInstances) 
		: Model("map", maxNoInstances, CONST_INSTANCES | NO_TEX) {}

	void init() override
	{
		loadModel("assets/models/map/map3.gltf");
	}

	void render(Shader shader, float dt, Scene* scene)
	{
		Model::render(shader, dt, scene);
	}
};

#endif