#pragma once
#include <vector>
#include <string>
#include "GraphicsIncludes.h"


struct Mesh
{
	// Cleanup/Init
	Mesh() = default;
	void Cleanup();
	enum UV_INPUT_TYPE 
	{
		POSITION = 0,
		NORMAL
	};

	enum UV_TYPE
	{
		PLANAR = 0,
		CYLINDRICAL,
		SPHERICAL,
		CUBE,
		MAX_UV_TYPE
	};

	// Functions
	void setUpMesh(const std::vector<Vertex>& verts, const std::vector<unsigned> indices);
	int setUpBuffers();
	int setUpNormalsBuffers();
	void normalize(const glm::mat4& scaleMat);

	glm::vec3 getModelScale() const;
	glm::vec3 getModelCentroid() const;

	int calcVertexNormals(bool flip = false);
	int calcVertexUVs();


	// Variables
	std::string name = "None";
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	
	
	std::vector<glm::vec3> vertNormalsDisplay;
	std::vector<glm::vec3> faceNormalsDisplay;

	GLuint VBO, EBO;
	GLuint normalsVBO[2] = {0,0};

	UV_INPUT_TYPE uv_input_type = POSITION;
	UV_TYPE uv_type = PLANAR;

	std::vector<glm::vec2> vertexUVs[2][MAX_UV_TYPE];
	GLuint uvsVBO[2][MAX_UV_TYPE] = { 0 };

	glm::vec3 boundingBox[2] = { glm::vec3{0.0f}, glm::vec3{0.0f} };

	// NOT IN USE!
	int vertexType = 0;
};