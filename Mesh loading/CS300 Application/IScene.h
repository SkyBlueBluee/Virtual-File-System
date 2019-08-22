#pragma once
#include <vector>
#include "GraphicsIncludes.h"
#include "Object.h"

// Abstract scene object. 
// Provides some basic functionality that other scenes that derive from it can use.
struct IScene
{
	virtual int Init() { return 0; }

	// This is called by the Graphics system.
	virtual int Display(float dt = 1.0f / 60.0f) 
	{ 
		if(PreRender(dt) || Render(dt) || PostRender(dt))
			return 1; 
		return 0;
	}
	virtual int PreRender(float dt = 1.0f / 60.0f) { return 0; }
	virtual int Render(float dt = 1.0f / 60.0f) { return 0; }
	virtual int PostRender(float dt = 1.0f / 60.0f) { return 0; }

	virtual int DisplayImGui() { return 0; }

	virtual void Shutdown() {}
	virtual ~IScene() {}

	glm::mat4 GetMVP(const glm::mat4 model);

	std::string sceneName;
	std::vector<Object> objects;
};