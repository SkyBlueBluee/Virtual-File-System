#include "Shader.h"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include "Engine.h"
#include "Graphics.h"

static const std::string PRAGMA_INCLUDE{ "#pragma include " };

Shader::~Shader()
{
	glDeleteProgram(programID);
}

std::string Shader::ReadShader(const std::string & shader, SHADER_TYPE type)
{
	std::string shaderCode;
	std::ifstream shaderStream(shader, std::ios::in);
	if (shaderStream.is_open()) {
		std::string Line;
		while (getline(shaderStream, Line))
		{
			size_t include = Line.find(PRAGMA_INCLUDE);
			size_t comment = Line.find("//");
			if (include != std::string::npos && comment > include)
			{
				std::string path = Engine::Instance()->GetSystem<Graphics>()->GetShaderPath();
				std::string file = Line.substr(include + PRAGMA_INCLUDE.length());
				switch (type)
				{
				case VERTEX_SHADER:
					if (vsIncludes.find(file) == vsIncludes.end())
					{
						vsIncludes.emplace(file, true);
						shaderCode += "\n// " + std::string{ "included file: " } + file;
						shaderCode += "\n" + ReadShader(path + file, type);
					}
					break;
				case FRAGMENT_SHADER:
					if (fsIncludes.find(file) == fsIncludes.end())
					{
						fsIncludes.emplace(file, true);
						shaderCode += "\n// " + std::string{ "included file: " } +file;
						shaderCode += "\n" + ReadShader(path + file, type);
					}
					break;
				}
			}
			else
				shaderCode += "\n" + Line;
		}
		shaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", shader.c_str());
		getchar();
		return std::string{ "" };
	}
	return shaderCode;
}

// DISCLAIMER: THIS WAS RIPPED FROM THE CODE IN THE SKELETON FRAMEWORK.
int Shader::InitShader(const std::string & vs, const std::string & fs)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode = ReadShader(vs, VERTEX_SHADER);
	
	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode = ReadShader(fs, FRAGMENT_SHADER);;

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vs.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);

#ifdef _DEBUG
		std::cout << "READ VERTEX SHADER: " << vs << std::endl << "=================================================" << std::endl;;
		std::cout << VertexShaderCode << std::endl << "=================================================" << std::endl;
#endif
	}


	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fs.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);

#ifdef _DEBUG
		std::cout << "READ VERTEX SHADER: " << vs << std::endl << "=================================================" << std::endl;;
		std::cout << VertexShaderCode << std::endl << "=================================================" << std::endl;
#endif
	}


	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	programID = ProgramID;

	type = PROGRAM;
	vsPath = vs;
	fsPath = fs;
	 
	return 0;
}

int Shader::Load(GLuint shaderID, const std::string & path)
{
	// Vertex Shader + Program
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line;
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", path.c_str());
		getchar();
		return 1;
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	programID = glCreateProgram();
	glAttachShader(programID, VertexShaderID);

	// IMP: Do this BEFORE linking the progam
	glProgramParameteri(programID,
						GL_PROGRAM_SEPARABLE,
						GL_TRUE);

	glLinkProgram(programID);
	return 0;
}
