#pragma once
#include <map>
#include <vector>
// #include "Mesh.h"
#include "GraphicsIncludes.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
	Vertex(const glm::vec3& pos = glm::vec3{ 0,0,0 },
		const glm::vec3& norm = glm::vec3{ 0,0,0 },
		const glm::vec3& text = glm::vec3{ 0,0,0 });
	glm::vec3 position = glm::vec3{ 0,0,0 };
	glm::vec3 normal = glm::vec3{ 0,0,0 };
	glm::vec2 uv = glm::vec2{ 0,0 };
};


struct Model
{
	Model();
	Model(const std::string& path);
	virtual ~Model();

	// Loading functions
	virtual int LoadModel();
	virtual int LoadModel(const std::string& path);

	// Draw functions
	int Draw();
	
	// Make the variables public for now. Will change to private when time permits.
	bool isLoaded = false;
	std::string name;
	std::string path;

protected:
	
	GLuint VAO = 0;

	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned> indices;

		GLuint vbo;
		GLuint ibo;
		unsigned texture_index;
	};
	std::vector<Mesh*> meshes;
	int blah;

	glm::vec3 minPos = glm::vec3{}, maxPos = glm::vec3{};

	void initMesh(const aiMesh* ai_mesh);
	void normalizeMeshes();
	void initMeshBuffers();
	void normalizeAndCalcMeshNormals();
	void updateBounds(const glm::vec3& pos, glm::vec3& minPos, glm::vec3& maxPos) const;
	
};