#pragma once
#include "GraphicsIncludes.h"

// NOT USED FOR NOW.
struct Framebuffer
{
	void Init(unsigned width, unsigned height);
	void Init(unsigned width, unsigned height, GLuint t);
	void Cleanup();

	void Use(size_t texture, bool clear = true);

	float width, height;
	GLuint fbo;	// Frame Buffer ID
	GLuint rbo;	// Render buffer object - holds depth data
	GLuint text[6];	// Texture ID
	size_t num_textures = 0;
};