#pragma once
#include <string>
#include <map>
#include "GraphicsIncludes.h"

struct Shader
{
	// Ctor/dtor
	Shader() = default;
	~Shader();

	int InitShader(const std::string & vs, const std::string & fs);
	void Use() { glUseProgram(programID); }
	
	// Utility Uniform Functions
	void SetBool(const std::string &name, bool value) const {
		glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
	}
	void SetInt(const std::string &name, int value) const {
		glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
	}
	void SetFloat(const std::string &name, float value) const {
		glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
	}
	void SetVec2(const std::string &name, const glm::vec2 &value) const {
		glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	void SetVec2(const std::string &name, float x, float y) const {
		glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
	}
	void SetVec3(const std::string &name, const glm::vec3 &value) const {
		glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	void SetVec3(const std::string &name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
	}
	void SetVec4(const std::string &name, const glm::vec4 &value) const {
		glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
	}
	void SetVec4(const std::string &name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
	}
	void SetMat2(const std::string &name, const glm::mat2 &mat) const {
		glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void SetMat3(const std::string &name, const glm::mat3 &mat) const {
		glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void SetMat4(const std::string &name, const glm::mat4 &mat) const {
		glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	// Not used.
	enum SHADER_TYPE
	{
		VERTEX_SHADER = 0,
		FRAGMENT_SHADER,
		PROGRAM
	};

	std::string name;

	GLuint programID;
	std::string vsPath;
	std::string fsPath;
	std::map<std::string, bool> vsIncludes, fsIncludes;
private:
	std::string ReadShader(const std::string& shader, SHADER_TYPE type);
	int Load(GLuint shaderID, const std::string& path);	// Not used for now.
	
	SHADER_TYPE type;	// Not used.
	std::string shader;// Not used.
	
};