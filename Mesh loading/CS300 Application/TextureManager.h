#pragma once
#include <map>
#include <string>

#include "GraphicsIncludes.h"

class TextureManager
{
public:
	TextureManager();
	int Init();
	int Shutdown();
	int LoadPNG(const std::string& filename, const std::string& saveAs);
	int LoadPPM(const std::string& filename, const std::string& saveAs);
	int LoadJPG(const std::string& filename, const std::string& saveAs);
	GLuint GetTexture(const std::string& name);
private:
	void loadSkybox();
	std::map<std::string, GLuint> textures;

};