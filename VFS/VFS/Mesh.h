#pragma once
#include "GraphicsIncludes.h" 
#include <vector>

struct Vertex
{
	glm::vec3 postion_;
	glm::vec3 tex_coords_;
	glm::vec3 normal_;
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::string& fname);
	void Init();


private:

	struct MeshEntry
	{
		MeshEntry(const std::vector<Vertex>& v, const std::vector<float>& i);

		std::vector<Vertex> vertices_;
		std::vector<float> indices_;
		
		GLuint vbo_;
		GLuint ibo_;
		size_t texture_index_;
	};

	std::string filename_;
	std::vector<MeshEntry> entries_;
	std::vector<GLuint> textures_;
};