#pragma once

#include "IScene.h"
#include "Light.h"
#include "Shader.h"
#include <string>
struct Model;
struct CS300_A1 : public IScene
{
	int Init();
	int PreRender(float dt = 1.0f / 60.0f);
	int Render(float dt = 1.0f / 60.0f);
	int PostRender(float dt = 1.0f / 60.0f);
	void Shutdown();

	int DisplayImGui() override;
	
private:
	int currMaxTexture = -1;
	// For drawing the objects
	void drawPhong(float dt);
	void setLightVariables(Shader* s);
	void drawLights();
	void drawLights(const glm::mat4 view, const glm::mat4 perps);
	void drawSkybox();
	void drawSkybox(const glm::mat4 view, const glm::mat4 perps);

	// Setting all light attributes
	void setUpSkybox();
	
	// What draw mode we are currently using.
	enum DRAW_MODE
	{
		FILL = 0,
		WIREFRAME,
		NONE
	};

	enum ATTENUATION
	{
		CONSTANT = 0,
		LINEAR,
		QUADRATIC
	};

	// For Editor
	std::vector<Model*> models;
	std::vector<Light> lights;
	
	std::string currModel;
	
	DRAW_MODE drawMode = FILL;
	
	// For Skybox
	GLuint skyboxVAO = 0;
	GLuint skyboxVBO = 0;
	
	// Material and Lighting attributes
	glm::vec3 globalAmbient		= { 0.2,0.2,0.2 };
	glm::vec3 fogColor			= { 0.5, 0.5, 0.5 };
	glm::vec3 materialAmbient	= { 0.2, 0.2, 0.2 };
	glm::vec3 materialEmissive	= { 0, 0,0 };
	float attenuation[3]		= { 1.0f, 0.1f, 0.01f };

	float zNear = 10.0f; 
	float zFar	= 30.0f;

	glm::vec3 all_ambient{ 0.1f,0.1f,0.1f }, all_diffuse{ 0.5f,0.5f,0.5f }, all_specular{ 1,1,1 };
	
	const std::pair<glm::vec3, glm::vec3> SKYBOX_DIRECTIONS[6]
	{
		{glm::vec3{-1,  0,  0}, glm::vec3{ 0,  1,  0}},
		{glm::vec3{ 1,  0,  0}, glm::vec3{ 0,  1,  0}},
		{glm::vec3{ 0, -1,  0}, glm::vec3{ 0,  0,  1}},
		{glm::vec3{ 0,  1,  0}, glm::vec3{ 0,  0, -1}},
		{glm::vec3{ 0,  0, -1}, glm::vec3{ 0,  1,  0}},
		{glm::vec3{ 0,  0,  1}, glm::vec3{ 0,  1,  0}}
	};
	
	std::vector<glm::vec3> skyboxVertices = {
		// positions          
		glm::vec3{-1.0f,  1.0f, -1.0f},
		glm::vec3{-1.0f, -1.0f, -1.0f},
		glm::vec3{ 1.0f, -1.0f, -1.0f},
		glm::vec3{ 1.0f, -1.0f, -1.0f},
		glm::vec3{ 1.0f,  1.0f, -1.0f},
		glm::vec3{-1.0f,  1.0f, -1.0f},
		
		glm::vec3{-1.0f, -1.0f,  1.0f},
		glm::vec3{-1.0f, -1.0f, -1.0f},
		glm::vec3{-1.0f,  1.0f, -1.0f},
		glm::vec3{-1.0f,  1.0f, -1.0f},
		glm::vec3{-1.0f,  1.0f,  1.0f},
		glm::vec3{-1.0f, -1.0f,  1.0f},

		glm::vec3{ 1.0f, -1.0f, -1.0f},
		glm::vec3{ 1.0f, -1.0f,  1.0f},
		glm::vec3{ 1.0f,  1.0f,  1.0f},
		glm::vec3{ 1.0f,  1.0f,  1.0f},
		glm::vec3{ 1.0f,  1.0f, -1.0f},
		glm::vec3{ 1.0f, -1.0f, -1.0f},

		glm::vec3{-1.0f, -1.0f,  1.0f},
		glm::vec3{-1.0f,  1.0f,  1.0f},
		glm::vec3{ 1.0f,  1.0f,  1.0f},
		glm::vec3{ 1.0f,  1.0f,  1.0f},
		glm::vec3{ 1.0f, -1.0f,  1.0f},
		glm::vec3{-1.0f, -1.0f,  1.0f},

		glm::vec3{-1.0f,  1.0f, -1.0f},
		glm::vec3{ 1.0f,  1.0f, -1.0f},
		glm::vec3{ 1.0f,  1.0f,  1.0f},
		glm::vec3{ 1.0f,  1.0f,  1.0f},
		glm::vec3{-1.0f,  1.0f,  1.0f},
		glm::vec3{-1.0f,  1.0f, -1.0f},
	
		glm::vec3{-1.0f, -1.0f, -1.0f},
		glm::vec3{-1.0f, -1.0f,  1.0f},
		glm::vec3{ 1.0f, -1.0f, -1.0f},
		glm::vec3{ 1.0f, -1.0f, -1.0f},
		glm::vec3{-1.0f, -1.0f,  1.0f},
		glm::vec3{ 1.0f, -1.0f,  1.0f}
	};
};
