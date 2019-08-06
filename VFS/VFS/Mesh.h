#pragma once
#include "GraphicsIncludes.h" 
#include "Texture.h"

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex
{
	glm::vec3 postion_;
	glm::vec3 normal_;
	glm::vec2 tex_coords_;
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::string& fname);
	void Init(const std::string& fname);
	void Render();

private:
	void initMeshEntry(const aiMesh* ai_mesh);
	void initMaterial(const aiMaterial* ai_material);

	struct MeshEntry
	{
		MeshEntry(const std::vector<Vertex>& v, const std::vector<unsigned>& i);

		std::vector<Vertex> vertices_;
		std::vector<unsigned> indices_;
		
		GLuint vbo_;
		GLuint ibo_;
		unsigned texture_index_;
	};

	std::string filename_;
	std::vector<MeshEntry*> entries_;
	std::vector<Texture*> textures_;
};