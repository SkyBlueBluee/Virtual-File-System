#include "Model.h"

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
#include <limits>
#include <vector>


Model::Model()
	:minPos{ INT_MAX,INT_MAX ,INT_MAX }, maxPos{ INT_MIN,INT_MIN ,INT_MIN}
{
}

Model::Model(const std::string & p)
	: Model()
{
	size_t start = p.find_last_of('\\') + 1;
	size_t end = p.find_last_of('.');
	name = p.substr(start, end - start);
	path = p;
}

Model::~Model()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(5, buffers);

}

int Model::LoadModel()
{
	return LoadModel(path);
}

int Model::LoadModel(const std::string & path)	
{
	Assimp::Importer importer;
	
	const aiScene* ai_scene = importer.ReadFile(path,
											 aiProcess_Triangulate |  // Triangulates non-triangles
											 aiProcess_GenSmoothNormals | // Generates missing normals
											 aiProcess_FlipUVs |			
											 aiProcess_JoinIdenticalVertices);

	if(ai_scene == nullptr )
	{
		std::cout	<< "Mesh Loading ERROR: \n"
					<< "\t Name:" << path << "\n"
					<< "\t Error: " << importer.GetErrorString() << std::endl;
		return -1;
	}
	
	// Initialize mesh entries
	initMeshes(ai_scene);
	isLoaded = true;

	return 0;
}

int Model::Draw()
{
	// Bind the current VAO
	glBindVertexArray(vao);

	for (unsigned int i = 0; i < meshes.size(); i++) {
		
		glDrawElementsBaseVertex( GL_TRIANGLES, 
								  static_cast<GLsizei>(meshes[i].num_index), 
								  GL_UNSIGNED_INT, 
								  (void*)(sizeof(unsigned) * meshes[i].base_index), 
								  meshes[i].base_vertex);
	}

	glBindVertexArray(0);
	return 0;
}

void Model::initMeshes(const aiScene* ai_scene)
{
	meshes.resize(ai_scene->mNumMeshes);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned> indices;

	unsigned num_vertices = 0, num_indices = 0;

	for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
	{
		meshes[i].base_vertex = num_vertices;
		meshes[i].base_index = num_indices;

		meshes[i].num_vertex = ai_scene->mMeshes[i]->mNumVertices;
		meshes[i].num_index = ai_scene->mMeshes[i]->mNumFaces * 3;

		num_vertices += meshes[i].num_vertex;
		num_indices += meshes[i].num_index;
	}

	positions.reserve(num_vertices);
	normals.reserve(num_vertices);
	uvs.reserve(num_vertices);
	indices.reserve(num_indices);

	for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[i];

		const aiVector3D  zero{ 0.0f, 0.0f, 0.0f };
		for (size_t i = 0; i < ai_mesh->mNumVertices; ++i)
		{
			const aiVector3D* pos = &(ai_mesh->mVertices[i]);
			const aiVector3D* normal = &(ai_mesh->mNormals[i]);
			const aiVector3D* text = ai_mesh->HasTextureCoords(0) ? &(ai_mesh->mTextureCoords[0][i]) : &zero;

			positions.emplace_back( pos->x, pos->y, pos->z );
			normals.emplace_back( normal->x, normal->y, normal->z );
			uvs.emplace_back(text->x, text->y);
			
			updateBounds(positions.back(), minPos, maxPos);
		}

		// Initialize indices
		for (size_t i = 0; i < ai_mesh->mNumFaces; i++)
		{
			const aiFace& face = ai_mesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
	}

	// Normalize all the positions
	normalizeMeshes(positions);
	initMeshBuffers(positions, normals, uvs, indices);

}

void Model::normalizeMeshes(std::vector<glm::vec3>& pos)
{
	// Normalizing the meshes
	glm::vec3 transVec = (maxPos + minPos) * 0.5f;
	glm::mat4 transMat = glm::translate(-transVec);

	glm::vec3 scaleVec = maxPos - minPos;
	float scaleFactor = std::max(scaleVec.x, std::max(scaleVec.y, scaleVec.z));

	scaleVec.x = 2.0f / scaleFactor;
	scaleVec.y = 2.0f / scaleFactor;
	scaleVec.z = 2.0f / scaleFactor;
	glm::mat4 scaleMat = glm::scale(scaleVec);

	glm::mat4 normalizeMat = scaleMat * transMat;

	// glm::mat4 bbMat = scaleMat * glm::translate(transVec);
	minPos = normalizeMat * glm::vec4{ minPos, 1.0f };
	maxPos = normalizeMat * glm::vec4{ maxPos, 1.0f };

	for (auto& p : pos)
	{
		p = normalizeMat * glm::vec4{ p, 1.0f };
	}
}

void Model::initMeshBuffers(const std::vector<glm::vec3>& pos, const std::vector<glm::vec3>& norm, const std::vector<glm::vec2>& uv, const std::vector<unsigned>& indices)
{
	// Create the VAO for the object itself
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Creating the VBO for vertices.
	glGenBuffers(5, buffers);

	// Creating the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);

	// Position
	glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3), pos.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Normal
	glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(glm::vec3), norm.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// UVs
	glBindBuffer(GL_ARRAY_BUFFER, buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), uv.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[POS_VB]);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3), pos.data(), GL_STATIC_DRAW);
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Position

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Model::updateBounds(const glm::vec3 & pos, glm::vec3& minPos, glm::vec3& maxPos)const
{
	// Update the bounding box size.
	if (pos.x < minPos.x)
		minPos.x = pos.x;
	if (pos.x > maxPos.x)
		maxPos.x = pos.x;

	if (pos.y < minPos.y)
		minPos.y = pos.y;
	if (pos.y > maxPos.y)
		maxPos.y = pos.y;

	if (pos.z < minPos.z)
		minPos.z = pos.z;
	if (pos.z > maxPos.z)
		maxPos.z = pos.z;
}

Vertex::Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec3& text)
	:position{pos}, normal{ norm }, uv{ text }
{
}
