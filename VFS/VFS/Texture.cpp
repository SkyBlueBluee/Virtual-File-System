#include "Texture.h"

#include <stb_image.h>

Texture::Texture(GLenum TextureTarget, const std::string & FileName)
	:texture_target_{ TextureTarget }, filename_{ FileName }
{
}

bool Texture::Load()
{
	int nrChannels;
	unsigned char *data = stbi_load((filename_).c_str(), &width_, &height_, &nrChannels, 0);
	if (data)
	{
		unsigned int texture_obj_;
		glGenTextures(1, &texture_obj_);
		glBindTexture(GL_TEXTURE_2D, texture_obj_);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		assert(false);
	}

	return false;
}

void Texture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(texture_target_, texture_obj_);
}
