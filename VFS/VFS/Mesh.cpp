#include "Mesh.h"
#include "VFS.h"

#include <iostream>


Mesh::Mesh(const std::string & fname)
	:filename_{fname}
{
}

void Mesh::Init(const std::string & fname)
{
	filename_ = fname;

	Assimp::Importer importer;
	
	const aiScene* ai_scene = importer.ReadFile(fname, 
											 aiProcess_Triangulate |  // Triangulates non-triangles
											 aiProcess_GenSmoothNormals | // Generates missing normals
											 aiProcess_FlipUVs |			
											 aiProcess_JoinIdenticalVertices);

	if(ai_scene == nullptr )
	{
		std::cout	<< "Mesh Loading ERROR: \n"
					<< "\t Name:" << filename_ << "\n"
					<< "\t Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	entries_.reserve(ai_scene->mNumMeshes);
	textures_.reserve(ai_scene->mNumTextures);

	// Initialize mesh entries
	for (size_t i = 0; i < entries_.size(); ++i)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[i];
		initMeshEntry(ai_mesh);
	}

	// Initialize textures
	for (unsigned int i = 0; i < ai_scene->mNumMaterials; i++)
	{
		const aiMaterial* ai_material = ai_scene->mMaterials[i];
		initMaterial(ai_material);
	}

}

void Mesh::Render()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0; i < entries_.size(); i++) {
		glBindBuffer(GL_ARRAY_BUFFER, entries_[i]->vbo_);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);	// pos
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(glm::vec3))); // norm
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(glm::vec3) * 2)); // uv

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entries_[i]->ibo_);

		const unsigned int MaterialIndex = entries_[i]->texture_index_;

		if (MaterialIndex < textures_.size() && textures_[MaterialIndex]) {
			textures_[MaterialIndex]->Bind(GL_TEXTURE0);
		}

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(entries_[i]->indices_.size()), GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void Mesh::initMeshEntry(const aiMesh * ai_mesh)
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
		
		vert.postion_ = glm::vec3{ pos->x, pos->y, pos->z };
		vert.normal_ = glm::vec3{ normal->x, normal->y, normal->z};
		vert.tex_coords_ = glm::vec2{ text->x, text->y };

		vertices.emplace_back(vert);
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

	MeshEntry* entry = new MeshEntry{ vertices, indices };
	entry->texture_index_ = ai_mesh->mMaterialIndex;

	// Creating the EBO
	glGenBuffers(1, &entry->ibo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entry->ibo_);
	glBufferData(	GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
					indices.data(), GL_STATIC_DRAW);

	// Creating the VBO for vertices.
	glGenBuffers(1, &entry->vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, entry->vbo_);
	glBufferData(	GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
					vertices.data(), GL_STATIC_DRAW);
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
	
	entries_.emplace_back(entry);
}

void Mesh::initMaterial(const aiMaterial * ai_material)
{
	std::string dir = "./resource/textures/";
	if (ai_material->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
	{
		aiString filename;
		if (ai_material->GetTexture(aiTextureType_DIFFUSE, 0, &filename, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			
			std::string path = dir + filename.data;
			Texture* texture = new Texture(GL_TEXTURE_2D, path.c_str());

			if (!texture->Load()) {
				printf("Error loading texture '%s'\n", path.c_str());
				delete texture;
				texture = nullptr;
			}

			textures_.push_back(texture);
		}
	}
	
}

Mesh::MeshEntry::MeshEntry(const std::vector<Vertex>& v, const std::vector<unsigned>& i)
	:vertices_{ std::move(v) }, indices_{std::move(i)}
{
}
