#include "Core.h"
#include "Graphics.h"
#include "VFS.h"

Core* Core::instance_ = new Core{};


void Core::Init()
{
	// Add all systems here
	graphics_ = new Graphics{};
	graphics_->Init();

	vfs_ = new VFS{};
	vfs_->Init();

	// Misc settings
	is_running_ = true;
}

void Core::Run()
{
	// Calculate deltatime
	float curr_time = static_cast<float>(glfwGetTime());
	dt_ = curr_time - prev_time_;
	prev_time_ = curr_time;

	auto* window = graphics_->GetWindow();

	// Checks if window needs to close
	if (glfwWindowShouldClose(window))
	{
		is_running_ = false;
		return;
	}
	
	// Update all systems.
	vfs_->Update(dt_);
	graphics_->Update(dt_);
	
	// Poll events and swap buffers
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Core::Shutdown()
{
	// Should cleanup opposite direction from adding
	graphics_->Shutdown();
	vfs_->Shutdown();
}

Core * Core::Get()
{
	return instance_;
}
