#include "InputSystem.h"

#include "Engine.h"
#include "Graphics.h"
#include "Camera.h"
#include "imgui.h"

// TO control mouse movement. On first mouse ensures we reset the offsets whenever our mouse first disappears (camera control)
static bool firstMouse = true;

// Keep track of prev mouse pos so that we can calc offset. This is in screen coords.
static float mousePrevX = Graphics::SCREEN_WIDTH_INIT / 2.0f;
static float mousePrevY = Graphics::SCREEN_HEIGHT_INIT / 2.0f;;

// Keep track of whether the cursor is showing or not. If the cursor is showing, we do not want to do anything in our application.
static bool showCursor = true;

// Keyboard TAB key callback. Makes the cursor disappear or appear.
static void ToggleShowCursorCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	
	// Show cursor when tab is pressed
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		if (!showCursor)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPos(window, graphics->GetWindowWidth() / 2.0f, graphics->GetWindowHeight() / 2.0f);
			showCursor = !showCursor;
		}
		else if(!ImGui::IsAnyWindowFocused())
		{
			// glfwSetCursorPos(window, graphics->GetWindowWidth() / 2.0f, graphics->GetWindowHeight() / 2.0f);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			showCursor = !showCursor;
			firstMouse = true;
		}
	}
}

// Mouse movement callback. Called whenver user moves the mouse.
static void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	// Do not do any inputs if cursor is shown
	if (showCursor)
		return;

	// Set the values directly if this is our first time entering.
	if (firstMouse == true)
	{
		mousePrevX = static_cast<float>(xpos);
		mousePrevY = static_cast<float>(ypos);
		firstMouse = false;
	}

	// Calculate offset
	float xoffset = static_cast<float>(xpos) - mousePrevX;
	float yoffset = mousePrevY - static_cast<float>(ypos); // reversed since y-coordinates go from bottom to top

	// Save curr pos as prev pos
	mousePrevX = static_cast<float>(xpos);
	mousePrevY = static_cast<float>(ypos);

	Engine::Instance()->GetSystem<Camera>()->MouseMovementInput(xoffset, yoffset);
}

// Scroll wheel callback. Makes the camera zoom in. Only occurs when cursor is not shown.
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Do not do any inputs if cursor is shown
	if (showCursor)
		return;
	Engine::Instance()->GetSystem<Camera>()->MouseScrollInput(static_cast<float>(yoffset));
}

int InputSystem::Init()
{
	
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	GLFWwindow* window = graphics->GetWindow();

	// Set up all the callbacks.
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, MouseScrollCallback);
	glfwSetKeyCallback(window, ToggleShowCursorCallback);
	return 0;
}

int InputSystem::Update(float dt)
{
	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	auto* window = graphics->GetWindow();

	// Check if specific inputs are pressed.

	// If ESC is pressed, we close the application.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Handle camera movement inputs only if cursor is not shown
	if (!showCursor)
	{
		Camera* camera = Engine::Instance()->GetSystem<Camera>();
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->MovementInput(Camera::FORWARD, dt);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->MovementInput(Camera::BACK, dt);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->MovementInput(Camera::LEFT, dt);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->MovementInput(Camera::RIGHT, dt);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera->MovementInput(Camera::UP, dt);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera->MovementInput(Camera::DOWN, dt);
	}

	return 0;
}

bool InputSystem::IsCursorShown() const
{
	return showCursor;
}
