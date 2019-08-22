#include "TextureManager.h"
#include <fstream>
#include <vector>
#include "stb_image.h"
static const std::string TEXTURE_PATH = "./resource/textures/";

TextureManager::TextureManager()
{
}

int TextureManager::Init()
{
	LoadPPM("metal_roof_spec_512x512.ppm", "A2_Specular");
	LoadPPM("metal_roof_diff_512x512.ppm", "A2_Diffuse");
	LoadJPG("checkerboard-rainbow.png", "Debug");
	LoadJPG("skybox/left.jpg", "left");
	loadSkybox();
	return 0;
}

int TextureManager::Shutdown()
{
	for (auto& texture : textures)
		glDeleteTextures(1, &texture.second);
	return 0;
}

int TextureManager::LoadPPM(const std::string & filename, const std::string& saveAs)
{
	// must be ppm format
	if (filename.substr(filename.size() - 4) != ".ppm")
		return 1;

	// Open the file
	std::ifstream file{TEXTURE_PATH + filename };
	if (!file.is_open())
		return 1;

	std::string line;
	// First line should state the ppm type, RAW ppm should be P6, PLAIN ppm should be P3
	std::getline(file, line);

	// We only read P3 files
	if (line != "P3")
		return 1;

	// Get rid of all comments after the magic number
	while (std::getline(file, line))
	{
		if (line.find('#') == std::string::npos)
			break;
	}

	// Read the width and height
	//std::getline(file, line);
	std::string width_str, height_str;
	size_t end = line.find_first_of(' ');
	width_str = line.substr(0, end);
	height_str = line.substr(end + 1);

	unsigned width = std::atoi(width_str.c_str());
	unsigned height = std::atoi(height_str.c_str());

	std::vector<float> data;
	data.reserve(width * height);
	for (unsigned i = 0; i < width * height; ++i)
	{
		std::string r_, g_, b_;
		file >> r_ >> g_ >> b_;
		float r, g, b;
		r = static_cast<float>(std::atoi(r_.c_str()) / 255.0f);
		g = static_cast<float>(std::atoi(g_.c_str()) / 255.0f);
		b = static_cast<float>(std::atoi(b_.c_str()) / 255.0f);

		data.emplace_back(r);
		data.emplace_back(g);
		data.emplace_back(b);
	}

	// Generate the texture buffers for PPM files
	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data.data());
	glGenerateMipmap(GL_TEXTURE_2D);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	textures.emplace(saveAs, textureID);
	return 0;
}

int TextureManager::LoadJPG(const std::string & filename, const std::string & saveAs)
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load((TEXTURE_PATH + filename).c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		textures.emplace(saveAs, texture);
	}
	else
	{
		assert(false);
	}
	stbi_image_free(data);
	return 0;
}

GLuint TextureManager::GetTexture(const std::string & name)
{
	auto result = textures.find(name);
	if (result != textures.end())
		return result->second;
	return 0;
}

void TextureManager::loadSkybox()
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	std::string faces[6] =
	{ 
		"skybox/right.jpg",
		"skybox/left.jpg",
		"skybox/top.jpg",
		"skybox/bottom.jpg",
		"skybox/front.jpg",
		"skybox/back.jpg" 
	};
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char *data = stbi_load((TEXTURE_PATH + faces[i]).c_str(), &width, &height, &nrChannels, 0);
		stbi_set_flip_vertically_on_load(false);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	textures.emplace("skybox", textureID);
}
