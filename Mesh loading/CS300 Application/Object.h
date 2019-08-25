#pragma once
#include <string>

#include "Model.h"
#include "Shader.h"
#include "GraphicsIncludes.h"

// Just a container to hold all the object data.
struct Object
{
	// Getters
	glm::mat4 GetModelMat();
	glm::mat4 GetModelMatInv();
	glm::mat4 GetNormalMat();
	
	// Draw functions
	int Draw(Shader* shader, float dt);
	
	// Varialbes
	glm::vec3 scale{1,1,1};
	glm::vec3 position{0,0,0};
	glm::vec3 axis{0,0,1};
	float angle = 0.0f;
	std::string modelID;
};