#include "Graphics.h"
#include "Engine.h"
#include "SceneManager.h"

// Error callback for GLFW
static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// When resizing, we want to keep track of the width and height within our system.
// Also call glViewport everytime the window is resized.
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Engine::Instance()->GetSystem<Graphics>()->SetWindowSize(width, height);
	glViewport(0, 0, width, height);
}

int Graphics::Init()
{
	// Setup window
	// Set a callback that glfw will call if there are errors
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Init opengl
	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(	windowWidth, windowHeight,
								"CS300 Application",
								nullptr, nullptr);
	// Error check
	if (window == NULL)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// glViewport(0, 0, windowWidth, windowHeight);
	// Initializing glew
	int err = glewInit() != GLEW_OK;
	if (err)
	{
		fprintf(stderr, "Failed to initialize glew due to: %s!\n", glewGetErrorString(err));
		return 1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Setting the background color
	bgColor = glm::vec4{ 0.5f, 0.4f, 0.60f, 1.00f };

	// Initialize the other graphics managers
	shaderManager.Init();
	modelManager.Init();
	textureManager.Init();
	framebufferManager.Init();

	return 0;
}

void Graphics::PreUpdate()
{
}

int Graphics::Update(float dt)
{
	PreUpdate();

	// Clear the color/depth buffer
	glfwMakeContextCurrent(window);
	glClearColor(bgColor.x, bgColor.y, bgColor.z, bgColor.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Display the current scene
	Engine::Instance()->GetSystem<SceneManager>()->GetActiveScene()->Display();

	PostUpdate();
	return 0;
}

void Graphics::PostUpdate()
{
	glfwMakeContextCurrent(window);
}

void Graphics::Shutdown()
{
	textureManager.Shutdown();
	shaderManager.Shutdown();
	framebufferManager.Shutdown();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Graphics::ErrorCheck()
{
	auto err = glGetError();
	if (err)
	{
		std::cout << "GlError: " << err << std::endl;
	}
}

Shader* Graphics::GetShader(const std::string & name)
{
	Shader* shader = shaderManager.GetShader(name);
	return shader;
}

Model* Graphics::GetModel( const std::string name)
{
	return modelManager.GetModel(name);
}

GLuint Graphics::UseShader(const std::string & name)
{
	Shader* shader = shaderManager.GetShader(name);
	if (shader == nullptr)
		return false;
	glUseProgram(shader->programID);
	return shader->programID;
}
