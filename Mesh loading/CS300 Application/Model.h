#pragma once
#include <map>
#include <vector>

// #include "Mesh.h"
#include "GraphicsIncludes.h"
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Model
{
	Model();
	Model(const std::string& path);
	virtual ~Model();

	// Loading functions
	virtual int LoadModel();
	virtual int LoadModel(const std::string& path);
	void BoneTransform(std::string anim_name, float time);

	// Draw functions
	int Draw(Shader* shader);
	
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
		BONE_VB,
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

	struct BoneInfo
	{
		glm::mat4 final_transform = glm::mat4{ 1.0f };
		glm::mat4 offset = glm::mat4{ 1.0f };
	};

	struct VertexBoneData
	{
		void addBoneData(int id, float weight);
		int ids[4]{};
		float weights[4]{};
	};

	using AiAnimNodeMap = std::map<std::string, const aiNodeAnim*>;
	struct AnimationSequence
	{
		AiAnimNodeMap node_map;
		float fps = -1.0f;
		float num_ticks = -1.0f;
		float duration = -1.0f;
	};
	
	Assimp::Importer importer;
	const aiScene* ai_scene = nullptr;
	
	GLuint vao = 0;
	GLuint buffers[NUM_VBs]{};
	GLuint ibo = 0;

	float elapsed = 0.0f;

	std::vector<Mesh> meshes;
	size_t num_bones = 0;
	std::map<std::string, int> bone_map;
	std::vector<BoneInfo> bone_info;
	std::vector<VertexBoneData> bone_data;
	
	std::map<std::string, AnimationSequence> animation_sequences;

	glm::mat4 global_inverse;
	glm::vec3 minPos = glm::vec3{}, maxPos = glm::vec3{};

	glm::mat4 initMat4(const aiMatrix4x4& mat);
	glm::mat4 initMat4(const aiMatrix3x3& mat);
	glm::vec3 initVec3(const aiVector3D& vec);
	void initMeshes();
	void initMeshBuffers(const std::vector<glm::vec3>& pos, const std::vector<glm::vec3>& norm, const std::vector<glm::vec2>& uv, const std::vector<unsigned>& indices);
	void initMeshBones(unsigned index, const aiMesh* mesh);
	void initAnimMaps();

	void recurseNodeHeirarchy(const AnimationSequence& anim_nodes, const aiNode* node, float time, const glm::mat4& parent_transform);

	template<typename T>
	unsigned aiFindKey(T* vec, unsigned size, float time)
	{
		for (unsigned i = 0; i < size - 1; ++i)
		{
			if (time < static_cast<float>(vec[i + 1].mTime))
			{
				return i;
			}
		}
		assert(0);

		return 0;
	}

	aiVector3D aiLerp(const aiVector3D& v1, const aiVector3D& v2, float factor);
	void normalizeMeshes(std::vector<glm::vec3>& pos);
	
	
	void updateBounds(const glm::vec3& pos, glm::vec3& minPos, glm::vec3& maxPos) const;
	
};