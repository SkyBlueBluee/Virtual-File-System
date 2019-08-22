#pragma once
#include <map>
#include <vector>
#include "Shader.h"

struct ShaderManager
{
	int Init();
	int Shutdown();
	~ShaderManager();
	const std::string& GetShaderPath() const;
	Shader* GetShader(const std::string& name);

	void ReloadShader(const std::string& name);

private:
	std::map<std::string, Shader*> shaders;
};