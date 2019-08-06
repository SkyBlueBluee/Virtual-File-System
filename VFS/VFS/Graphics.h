#pragma once
#include "GraphicsIncludes.h"


class Graphics
{
public:
	void Init();
	void Update(float dt);
	void Shutdown();

	auto GetWindow()->GLFWwindow* { return window_; };
	int GetWindowWidth() const { return static_cast<int>(window_width_); }
	int GetWindowHeight() const { return static_cast<int>(window_height_); }

	void SetWindowSize(float width, float height) { window_width_ = width, window_height_ = height; }

	
private:
	GLFWwindow* window_;
	float window_width_ = 1200.0f, window_height_ = 800.0f;

	glm::vec4 bg_color_;
	// Private Methods
	void PreUpdate();
	void PostUpdate();
};