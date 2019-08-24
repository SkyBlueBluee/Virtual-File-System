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
	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		// BONE_VB,
		NUM_VBs
	};

	struct Mesh
	{
		// std::vector<Vertex> vertices;
		// std::vector<unsigned> indices;
		unsigned base_vertex;
		unsigned base_index;
		unsigned num_vertex;
		unsigned num_index;
		unsigned texture_index;
	};

	struct BoneData
	{
		void addBoneData(int id, float weight);
		int ids[4]{};
		float weights[4]{};
	};

	GLuint vao = 0;
	GLuint buffers[NUM_VBs]{};
	GLuint ibo = 0;

	std::vector<Mesh> meshes;
	std::map<std::string, int> bone_map;
	std::vector<BoneData> bone_data;

	glm::vec3 minPos = glm::vec3{}, maxPos = glm::vec3{};

	void initMeshes(const aiScene* ai_scene);
	void normalizeMeshes(std::vector<glm::vec3>& pos);
	void initMeshBuffers(const std::vector<glm::vec3>& pos, const std::vector<glm::vec3>& norm, const std::vector<glm::vec2>& uv, const std::vector<unsigned>& indices);
	void normalizeAndCalcMeshNormals();
	void updateBounds(const glm::vec3& pos, glm::vec3& minPos, glm::vec3& maxPos) const;
	
};