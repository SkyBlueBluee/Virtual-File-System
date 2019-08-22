#include "ShaderManager.h"


static const std::string SHADER_PATH = "./resource/shader/";


int ShaderManager::Init()
{
	// Load default shader here
	Shader* shader = new Shader{};
	shader->InitShader(SHADER_PATH + "default.vert", SHADER_PATH + "default.frag");
	shaders.emplace("default", shader);

	shader = new Shader{};
	shader->InitShader(SHADER_PATH + "normals.vert", SHADER_PATH + "normals.frag");
	shaders.emplace("normals", shader);

	shader = new Shader{};
	shader->InitShader(SHADER_PATH + "PhongS.vert", SHADER_PATH + "PhongS.frag");
	shaders.emplace("PhongS", shader);

	shader = new Shader{};
	shader->InitShader(SHADER_PATH + "PhongL.vert", SHADER_PATH + "PhongL.frag");
	shaders.emplace("PhongL", shader);

	shader = new Shader{};
	shader->InitShader(SHADER_PATH + "Environment Map.vert", SHADER_PATH + "Environment Map.frag");
	shaders.emplace("Environment", shader);

	shader = new Shader{};
	shader->InitShader(SHADER_PATH + "debug.vert", SHADER_PATH + "debug.frag");
	shaders.emplace("debug", shader);

	shader = new Shader{};
	shader->InitShader(SHADER_PATH + "skybox.vert", SHADER_PATH + "skybox.frag");
	shaders.emplace("skybox", shader);

	return 0;
}

int ShaderManager::Shutdown()
{
	// Cleanup
	for (auto& elem : shaders)
	{
		delete elem.second;
	}
	return 0;
}

ShaderManager::~ShaderManager()
{
	
}

const std::string& ShaderManager::GetShaderPath() const
{
	return SHADER_PATH;
}

Shader * ShaderManager::GetShader(const std::string & name)
{
	auto result = shaders.find(name);
	if(result == shaders.end())
		return nullptr;
	return result->second;
}

void ShaderManager::ReloadShader(const std::string & name)
{
	auto result = shaders.find(name);
	if (result != shaders.end())
	{
		glDeleteProgram(result->second->programID);
		result->second->vsIncludes.clear();
		result->second->fsIncludes.clear();
		result->second->InitShader(result->second->vsPath, result->second->fsPath);
	}
}
