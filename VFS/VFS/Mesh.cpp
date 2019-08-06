#include "Mesh.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Mesh::Mesh(const std::string & fname)
	:filename_{fname}
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(fname, 
								aiProcess_Triangulate |  // Triangulates non-triangles
								aiProcess_GenSmoothNormals | // Generates missing normals
								aiProcess_FlipUVs |			
								aiProcess_JoinIdenticalVertices);

	if( scene == nullptr )
	{
		std::cout	<< "Mesh Loading ERROR: \n"
					<< 	"\t Name:" << filename_ << "\n"
					<< "\t Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	entries_.reserve(scene->mNumMeshes);
	textures_.reserve(scene->mNumTextures);

	for (size_t i = 0; i < entries_.size(); ++i)
	{
		aiMesh* ai_mesh = scene->mMeshes[i];


	}

}

Mesh::MeshEntry::MeshEntry(const std::vector<Vertex>& v, const std::vector<float>& i)
	:vertices_{ std::move(v) }, indices_{std::move(i)}
{
}
