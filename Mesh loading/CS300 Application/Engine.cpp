#include "Engine.h"
#include "Graphics.h"
#include "Editor.h"
#include "SceneManager.h"
#include "Camera.h"
#include "InputSystem.h"

Engine* Engine::instance = new Engine{};


int Engine::Init()
{
	// Add all systems here

	systems_vec.reserve(5);		// At least 5 systems
	AddSystem<Graphics>();		// Should be first as graphics will create context

	// ImGui will call our previously binded glfw input callbacks. Hence it is important that InputSystem is initialized first.
	AddSystem<InputSystem>();
	AddSystem<Editor>();
	AddSystem<SceneManager>();
	AddSystem<Camera>();
	
	// Call init on all systems
	// Systems initialized in order that they were added.
	for (auto* elem : systems_vec)
	{
		if (elem->Init())
			return 1;
	}

	// Misc settings
	isRunning = true;

	return 0;
}

void Engine::Run()
{
	// Calculate deltatime
	float currentFrame = static_cast<float>(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	auto* window = GetSystem<Graphics>()->GetWindow();

	// Checks if window needs to close
	if (glfwWindowShouldClose(window))
	{
		isRunning = false;
		return;
	}

	// Update all systems.
	for (auto* elem : systems_vec)
	{
		if (elem->Update(deltaTime))
		{
			isRunning = false;
			return;
		}
	}

	// Poll events and swap buffers
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Engine::Shutdown()
{
	// Should cleanup opposite direction from adding
	auto end = systems_vec.rend();
	for (auto start = systems_vec.rbegin(); start != end; ++start)
	{
		(*start)->Shutdown();
	}
}