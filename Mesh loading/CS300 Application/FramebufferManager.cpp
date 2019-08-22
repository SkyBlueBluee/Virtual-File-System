#include "FramebufferManager.h"
#include "Engine.h"
#include "Graphics.h"
#include "TextureManager.h"
int FrameBufferManager::Init()
{
	const float width  = Engine::Instance()->GetSystem<Graphics>()->GetWindowWidth();
	const float height = Engine::Instance()->GetSystem<Graphics>()->GetWindowHeight();

	// Initialize the skybox framebuffers
	Framebuffer* fb = nullptr;
	
	fb = new Framebuffer{};
	fb->Init(500, 500);
	framebuffers.emplace("Environment" , fb);

	// Revert back to the original fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return 0;
}

int FrameBufferManager::Shutdown()
{
	for (auto& fb : framebuffers)
	{
		fb.second->Cleanup();
		delete fb.second;
	}
	return 0;
}

Framebuffer* FrameBufferManager::GetFramebuffer(const std::string& name)
{
	auto result = framebuffers.find(name);
	if (result == framebuffers.end())
		return nullptr;
	return result->second;
}

Framebuffer * FrameBufferManager::UseFramebuffer(const std::string & name)
{
	Framebuffer* fb = GetFramebuffer(name);
	if (fb)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
		glViewport(0, 0, fb->width, fb->height);
	}
	return fb;
}

Framebuffer* FrameBufferManager::UseFramebuffer(const std::string & name, size_t texture)
{
	Framebuffer* fb = GetFramebuffer(name);
	if (fb)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
		fb->Use(texture);
		glViewport(0, 0, fb->width, fb->height);
	}
	return fb;
}

void FrameBufferManager::UseFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Engine::Instance()->GetSystem<Graphics>()->GetWindowWidth(), Engine::Instance()->GetSystem<Graphics>()->GetWindowHeight());
	glDrawBuffer(GL_BACK_LEFT);
}
