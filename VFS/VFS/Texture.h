#pragma once
#include <string>

#include "GraphicsIncludes.h"

class Texture
{
public:
	Texture(GLenum TextureTarget, const std::string& FileName);

	bool Load();

	void Bind(GLenum TextureUnit);

private:
	std::string filename_;
	GLenum texture_target_;
	GLuint texture_obj_;

	int width_ = 0, height_ = 0;
};