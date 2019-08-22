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
	size_t end = p.length() - 4;
	name = p.substr(start, end - start);
	path = p;
}

Model::~Model()
{
	glDeleteVertexArrays(1, &VAO);
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
	
	meshes.reserve(ai_scene->mNumMeshes);
	
	// Initialize mesh entries
	for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[i];
		initMesh(ai_mesh);
	}

	normalizeMeshes();
	initMeshBuffers();
	isLoaded = true;

	return 0;
}

int Model::Draw()
{
	// Bind the current VAO
	glBindVertexArray(VAO);

	for (unsigned int i = 0; i < meshes.size(); i++) {
		glBindBuffer(GL_ARRAY_BUFFER, meshes[i]->vbo);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);	// pos
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3))); // norm
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) * 2)); // uv

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->ibo);

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(meshes[i]->indices.size()), GL_UNSIGNED_INT, 0);
	}
	
	return 0;
}

void Model::initMesh(const aiMesh* ai_mesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;

	vertices.reserve(ai_mesh->mNumVertices);
	indices.reserve(ai_mesh->mNumFaces);
	

	const aiVector3D  zero{ 0.0f, 0.0f, 0.0f };
	for (size_t i = 0; i < ai_mesh->mNumVertices; ++i)
	{
		const aiVector3D* pos = &(ai_mesh->mVertices[i]);
		const aiVector3D* normal = &(ai_mesh->mNormals[i]);
		const aiVector3D* text = ai_mesh->HasTextureCoords(0) ? &(ai_mesh->mTextureCoords[0][i]) : &zero;
		
		Vertex vert;
		
		vert.position = glm::vec3{ pos->x, pos->y, pos->z };
		vert.normal = glm::vec3{ normal->x, normal->y, normal->z};
		vert.uv = glm::vec2{ text->x, text->y };

		vertices.emplace_back(vert);

		updateBounds(vert.position, minPos, maxPos);
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

	Mesh* entry = new Mesh{ vertices, indices };

	meshes.emplace_back(entry);
}

void Model::normalizeMeshes()
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

	for (auto& mesh : meshes)
	{
		for (auto& vert : mesh->vertices)
		{
			vert.position = normalizeMat * glm::vec4{ vert.position, 1.0f };
		}
	}
}

void Model::initMeshBuffers()
{
	// Create the VAO for the object itself
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	for (auto& entry : meshes)
	{
			// Creating the EBO
		glGenBuffers(1, &entry->ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entry->ibo);
		glBufferData(	GL_ELEMENT_ARRAY_BUFFER, entry->indices.size() * sizeof(unsigned),
						entry->indices.data(), GL_STATIC_DRAW);

		// Creating the VBO for vertices.
		glGenBuffers(1, &entry->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, entry->vbo);
		glBufferData(	GL_ARRAY_BUFFER, entry->vertices.size() * sizeof(Vertex),
						entry->vertices.data(), GL_STATIC_DRAW);
		// Position
		glVertexAttribPointer(	0,
								3,
								GL_FLOAT,
								GL_FALSE,
								sizeof(Vertex),
								(void *)(0));
		glEnableVertexAttribArray(0);

		// Normals
		glVertexAttribPointer(	1,
								3,
								GL_FLOAT,
								GL_FALSE,
								sizeof(Vertex),
								(void *)(sizeof(glm::vec3)));
		glEnableVertexAttribArray(1);

		// UVs
		glVertexAttribPointer(	2,
								2,
								GL_FLOAT,
								GL_FALSE,
								sizeof(Vertex),
								(void *)(sizeof(glm::vec3) * 2));
		glEnableVertexAttribArray(2);
	}
	
}

void Model::normalizeAndCalcMeshNormals()
{
	

	
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
