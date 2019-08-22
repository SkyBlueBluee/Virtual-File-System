#pragma once
#include <map>
#include <string>
#include "GraphicsIncludes.h"
#include "Framebuffer.h"

class FrameBufferManager
{
public:
	int Init();
	int Shutdown();

	Framebuffer* GetFramebuffer(const std::string& name);
	Framebuffer* UseFramebuffer(const std::string& name);
	Framebuffer* UseFramebuffer(const std::string& name, size_t texutre);
	void UseFrameBuffer();
private:
	std::map<std::string, Framebuffer*> framebuffers;
};