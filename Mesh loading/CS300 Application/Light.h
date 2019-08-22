#pragma once
#include "Object.h"

struct Light : public Object
{
	enum LIGHT_TYPE
	{
		POINT = 0,
		DIRECTIONAL,
		SPOTLIGHT
	};

	Light() = default;


	bool isOn = false;
	LIGHT_TYPE type = POINT;
	glm::vec3 direction;
	glm::vec3 ambient{0.1f,0.1f,0.1f}, diffuse{ 0.5f,0.5f,0.5f }, specular{ 1,1,1};
	float innerAngle = 15.0f, outerAngle = 30.0f, spotFalloff = 1.0f;
};
