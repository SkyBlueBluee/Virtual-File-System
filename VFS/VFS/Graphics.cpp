#include "Graphics.h"
#include "Core.h"
#include <iostream>

// Error callback for GLFW
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// When resizing, we want to keep track of the width and height within our system.
// Also call glViewport everytime the window is resized.
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Core::Get()->GetGraphics()->SetWindowSize(width, height);
	glViewport(0, 0, width, height);
}


void Graphics::Init()
{
	// Setup window
	// Set a callback that glfw will call if there are errors
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return;

	// Init opengl
	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window_ = glfwCreateWindow(	window_width_, window_height_,
								"VFS & Animation Demo",
								nullptr, nullptr);
	// Error check
	if (window_ == NULL)
		return;

	glfwMakeContextCurrent(window_);
	glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
	// glViewport(0, 0, windowWidth, windowHeight);
	// Initializing glew
	int err = glewInit() != GLEW_OK;
	if (err)
	{
		fprintf(stderr, "Failed to initialize glew due to: %s!\n", glewGetErrorString(err));
		return;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);

	// Setting the background color
	bg_color_ = glm::vec4{ 0.5f, 0.4f, 0.60f, 1.00f };

	// Initialize the other graphics managers
	
	std::cout << "Graphics System Init - COMPLETED" << std::endl;
}


void Graphics::Update(float dt)
{
	PreUpdate();

	// Clear the color/depth buffer
	glfwMakeContextCurrent(window_);
	glClearColor(bg_color_.x, bg_color_.y, bg_color_.z, bg_color_.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Display the current scene
	

	glfwMakeContextCurrent(window_);
	return;
}

void Graphics::Shutdown()
{
	
	glfwDestroyWindow(window_);
	glfwTerminate();
}
