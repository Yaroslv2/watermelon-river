#ifndef TREE_HPP
#define TREE_HPP

#include "../objects/model.h"

class Tree
	: public Model
{
public:
	Tree(unsigned int maxNoInstances)
		: Model("tree" + std::to_string(Tree::idx), maxNoInstances, CONST_INSTANCES | NO_TEX) 
	{
		Tree::idx += 1;
	}

	void init() override
	{
		loadModel("assets/models/tree/tree.gltf");
	}

	void render(Shader shader, float dt, Scene* scene)
	{
		Model::render(shader, dt, scene);
	}

private:
	static int idx;
};

int Tree::idx = 0;

#endif