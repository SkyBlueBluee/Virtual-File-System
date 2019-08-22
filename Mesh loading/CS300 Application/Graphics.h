#pragma once
#include <vector>
#include <map>

// Wrapper for all the opengl headers
#include "GraphicsIncludes.h"

#include "ISystem.h"
#include "Framebuffer.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "FramebufferManager.h"
class Graphics :public ISystem
{
public:
	// The default screen size that the window is initialized to be.
	static constexpr int SCREEN_WIDTH_INIT = 1200;
	static constexpr int SCREEN_HEIGHT_INIT = 800;

	// System functions
	int Init() override;
	int Update(float dt) override;
	void Shutdown() override;
	static void ErrorCheck();

	// Aux functions
	
	GLuint UseShader(const std::string& name);
	void RenderSkyBox();

	// Getters
	GLFWwindow* GetWindow() { return window; }
	int GetWindowWidth() const { return windowWidth; }
	int GetWindowHeight() const { return windowHeight; }

	glm::vec4 GetBGColor() const { return bgColor; }

	ModelManager const* GetModelManager() const { return &modelManager; }
	ModelManager* GetModelManager() { return &modelManager; }
	Model* GetModel(const std::string name);

	Shader* GetShader(const std::string& name);
	std::string GetShaderPath() const { return shaderManager.GetShaderPath(); }

	// Setters
	void SetWindowSize(int w, int h) { windowWidth = w, windowHeight = h; }
	void SetBGColor(const glm::vec4& col) { bgColor = col; }
	
	// Shaders
	ShaderManager shaderManager;
	// std::map<std::string, Shader> vertexShaders;
	// std::map<std::string, Shader> fragmentShaders;

	// Models
	ModelManager modelManager;

	// Textures
	TextureManager textureManager;

	// FrameBuffers
	FrameBufferManager framebufferManager;

private:
	// Private Methods
	void PreUpdate();
	void PostUpdate();

	// glfw specific variables
	// Width and height of the window. Can change.
	GLFWwindow* window = nullptr;
	int windowWidth = SCREEN_WIDTH_INIT;
	int windowHeight = SCREEN_HEIGHT_INIT;
	glm::vec4 bgColor;
};