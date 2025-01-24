#ifndef WATERMELON_HPP
#define WATERMELON_HPP

#include "../objects/model.h"

class Watermelon
	: public Model
{
public:
	Watermelon(unsigned int maxNoInstances)
		: Model("watermelon" + std::to_string(Watermelon::idx), maxNoInstances, NO_TEX | DYNAMIC)
	{
		Watermelon::idx += 1;
	}

	void init()
	{
		loadModel("assets/models/watermelon/watermelon.gltf");

		//std::vector<Texture> textures = {
		//	Texture("assets/textures", "brickwall_diffuse.jpg", aiTextureType_DIFFUSE),
		//	Texture("assets/textures", "brickwall_normal.jpg", aiTextureType_NORMALS),
		//	Texture("assets/textures", "brickwall_specular.jpg", aiTextureType_SPECULAR)
		//};

		//int noVertices = 4;

		//float quadVertices[] = {
		//	// position			normal				texcoord
		//	 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top right
		//	-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
		//	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
		//	 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 0.0f  // bottom right
		//};
		//float collisionVertices[] = {
		//	// position			
		//	 0.5f,  0.5f, 0.0f,
		//	-0.5f,  0.5f, 0.0f,
		//	-0.5f, -0.5f, 0.0f,
		//	 0.5f, -0.5f, 0.0f
		//};

		//unsigned int indices[] = {
		//	0, 1, 3,
		//	1, 2, 3
		//};

		//BoundingRegion br(glm::vec3(0.0f), 1 / sqrt(2.0f));

		//Mesh ret = processMesh(br,
		//	noVertices, quadVertices,
		//	6, indices,
		//	true,
		//	noVertices, collisionVertices,
		//	2, indices);

		//ret.setupTextures(textures);

		//addMesh(&ret);
	}
private:
	static int idx;
};

int Watermelon::idx = 0;

#endif