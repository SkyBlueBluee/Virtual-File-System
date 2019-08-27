#include "Model.h"

#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <algorithm>
#include <limits>
#include <vector>

#include "Engine.h"
#include "Graphics.h"
#include "ShaderManager.h"
#include "Shader.h"


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
	
	
	ai_scene = importer.ReadFile(path,
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

	global_inverse = initMat4(ai_scene->mRootNode->mTransformation);
	global_inverse = glm::inverse(global_inverse);
	
	
	// Initialize mesh entries
	initMeshes();
	isLoaded = true;

	return 0;
}

void Model::BoneTransform(std::string anim_name, float time)
{
	auto anim_seq = animation_sequences.begin();
	if (anim_seq == animation_sequences.end())
		return;

	elapsed += time;

	float ticks = elapsed * anim_seq->second.fps;
	float anim_time = fmod(ticks, anim_seq->second.num_ticks);
	glm::mat4 mat{ 1.0f };
	recurseNodeHeirarchy(anim_seq->second, ai_scene->mRootNode, anim_time, mat);
}

int Model::Draw(Shader* shader)
{
	for (unsigned i = 0; i < bone_info.size(); ++i)
	{
		std::string name = "Bones[" + std::to_string(i) + "]";
		shader->SetMat4(name, bone_info[i].final_transform);
	}

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

void Model::initMeshes()
{
	meshes.resize(ai_scene->mNumMeshes);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned> indices;

	unsigned num_vertices = 0, num_indices = 0;

	for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
	{
		meshes[i].num_index = ai_scene->mMeshes[i]->mNumFaces * 3;
		meshes[i].base_vertex = num_vertices;
		meshes[i].base_index = num_indices;

		num_vertices += ai_scene->mMeshes[i]->mNumVertices;
		num_indices += meshes[i].num_index;
	}

	positions.reserve(num_vertices);
	normals.reserve(num_vertices);
	uvs.reserve(num_vertices);
	bone_data.resize(num_vertices);

	indices.reserve(num_indices);
	

	for (size_t i = 0; i < ai_scene->mNumMeshes; ++i)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[i];

		const aiVector3D  zero{ 0.0f, 0.0f, 0.0f };
		for (size_t k = 0; k < ai_mesh->mNumVertices; ++k)
		{
			const aiVector3D* pos = &(ai_mesh->mVertices[k]);
			const aiVector3D* normal = &(ai_mesh->mNormals[k]);
			const aiVector3D* text = ai_mesh->HasTextureCoords(0) ? &(ai_mesh->mTextureCoords[0][k]) : &zero;

			positions.emplace_back( pos->x, pos->y, pos->z );
			normals.emplace_back( normal->x, normal->y, normal->z );
			uvs.emplace_back(text->x, text->y);
			
			updateBounds(positions.back(), minPos, maxPos);
		}

		// Initialize indices
		for (size_t k = 0; k < ai_mesh->mNumFaces; k++)
		{
			const aiFace& face = ai_mesh->mFaces[k];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		initMeshBones(i, ai_scene->mMeshes[i]);
	}

	// Normalize all the positions
	// normalizeMeshes(positions);
	initMeshBuffers(positions, normals, uvs, indices);
	
	initAnimMaps();
}

glm::mat4 Model::initMat4(const aiMatrix4x4& mat)
{
	glm::mat4 m{1.0f};
	for (unsigned i = 0; i < 4; ++i)
	{
		glm::vec4 v{ mat[0][i], mat[1][i] ,mat[2][i] ,mat[3][i] };
		m[i] = v;
	}
	return m;
}

glm::mat4 Model::initMat4(const aiMatrix3x3& mat)
{
	glm::mat4 m{ 1.0f };
	for (unsigned i = 0; i < 3; ++i)
	{
		glm::vec4 v{ mat[0][i], mat[1][i] ,mat[2][i], 0.0f };
		m[i] = v;
	}
	return m;
}

glm::vec3 Model::initVec3(const aiVector3D& vec)
{
	return glm::vec3(vec.x, vec.y, vec.z);
}

void Model::recurseNodeHeirarchy(const AnimationSequence& anim_nodes, const aiNode* node, float time, const glm::mat4& parent_transform)
{
	std::string node_name = node->mName.data;
	glm::mat4 node_transform = initMat4(node->mTransformation);

	auto result = anim_nodes.node_map.find(node_name);
	if (result != anim_nodes.node_map.end())
	{
		const aiNodeAnim* anim_node = result->second;
		
		aiVector3D scale		= anim_node->mScalingKeys[0].mValue;
		aiVector3D translate	= anim_node->mPositionKeys[0].mValue;
		aiQuaternion rotate		= anim_node->mRotationKeys[0].mValue;

		// Scaling
		if (anim_node->mNumScalingKeys > 1)
		{
			unsigned start = aiFindKey(anim_node->mScalingKeys, anim_node->mNumScalingKeys, time);
			assert(start + 1 < anim_node->mNumScalingKeys);
			float dt = static_cast<float>(anim_node->mScalingKeys[start + 1].mTime - anim_node->mScalingKeys[start].mTime);
			float factor = static_cast<float>(time - anim_node->mScalingKeys[start].mTime / dt);
			assert(factor >= 0.0f && factor <= 1.0f);
			scale = aiLerp(anim_node->mScalingKeys[start].mValue, anim_node->mScalingKeys[start + 1].mValue, factor);
		}

		// Translate
		if (anim_node->mNumPositionKeys > 1)
		{
			unsigned start = aiFindKey(anim_node->mPositionKeys, anim_node->mNumPositionKeys, time);
			assert(start + 1 < anim_node->mNumPositionKeys);
			float dt = static_cast<float>(anim_node->mPositionKeys[start + 1].mTime - anim_node->mPositionKeys[start].mTime);
			float factor = static_cast<float>(time - anim_node->mPositionKeys[start].mTime / dt);
			assert(factor >= 0.0f && factor <= 1.0f);
			translate = aiLerp(anim_node->mPositionKeys[start].mValue, anim_node->mPositionKeys[start + 1].mValue, factor);
		}

		// Rotate
		if (anim_node->mNumRotationKeys > 1)
		{
			unsigned start = aiFindKey(anim_node->mRotationKeys, anim_node->mNumRotationKeys, time);
			assert(start + 1 < anim_node->mNumRotationKeys);
			float dt = static_cast<float>(anim_node->mRotationKeys[start + 1].mTime - anim_node->mRotationKeys[start].mTime);
			float factor = static_cast<float>(time - anim_node->mRotationKeys[start].mTime / dt);
			assert(factor >= 0.0f && factor <= 1.0f);
			const aiQuaternion& q1 = anim_node->mRotationKeys[start].mValue;
			const aiQuaternion& q2 = anim_node->mRotationKeys[start + 1].mValue;
			aiQuaternion::Interpolate(rotate, q1, q2, factor);
			rotate.Normalize();
		}
		
		
		const glm::mat4 scale_m = glm::scale(glm::vec3{ scale.x, scale.y, scale.z });
		const glm::mat4 trans_m = glm::translate(glm::vec3{ translate.x, translate.y, translate.z });
		aiMatrix3x3 mat = rotate.GetMatrix();
		const glm::mat4 rotate_m = initMat4(mat);

		node_transform = trans_m * rotate_m * scale_m;
	}

	glm::mat4 global_transform = parent_transform * node_transform;

	auto bone_res = bone_map.find(node_name);
	if (bone_res != bone_map.end())
	{
		glm::vec3 v{ -1, 1, -1 };
		glm::mat4 m = glm::rotate(glm::radians(45.0f), glm::vec3{ 1, 0, 0 });
		glm::mat4 test = glm::translate(glm::vec3{ 0, 5, 0 }) * m * glm::translate(glm::vec3{ 0, -5, 0 });
		v = (test * glm::vec4{ v, 1.0f });
		bone_info[bone_res->second].final_transform = global_inverse * parent_transform * node_transform * bone_info[bone_res->second].offset;
	}

	for (unsigned i = 0; i < node->mNumChildren; ++i)
	{
		recurseNodeHeirarchy(anim_nodes, node->mChildren[i], time, global_transform);
	}
}


aiVector3D Model::aiLerp(const aiVector3D& v1, const aiVector3D& v2, float factor)
{
	return v1 + ((v2 - v1) * factor);
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
	glGenBuffers(NUM_VBs, buffers);

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

	glBindBuffer(GL_ARRAY_BUFFER, buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, bone_data.size() * sizeof(VertexBoneData), bone_data.data(), GL_STATIC_DRAW);
	
	// IDs
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	 
	// Weights
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
}

void Model::initMeshBones(unsigned index, const aiMesh* mesh)
{
	for (unsigned i = 0; i < mesh->mNumBones; i++) 
	{
		unsigned curr_index = 0;
		std::string name(mesh->mBones[i]->mName.data);
		auto result_bone = bone_map.find(name);
		if (result_bone == bone_map.end()) 
		{
			curr_index = num_bones;
			num_bones++;
			BoneInfo bi;
			
			bi.offset = initMat4(mesh->mBones[i]->mOffsetMatrix);
			bone_info.push_back(bi);
			bone_map.emplace(name, curr_index);
		}
		else {
			curr_index = result_bone->second;
		}
		
		for (unsigned j = 0; j < mesh->mBones[i]->mNumWeights; j++) 
		{
			unsigned vert_id = meshes[index].base_vertex + mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bone_data[vert_id].addBoneData(curr_index, weight);
		}
	}
}

void Model::initAnimMaps()
{
	const aiNode* node = ai_scene->mRootNode;
	for (unsigned i = 0; i < ai_scene->mNumAnimations; ++i)
	{
		const aiAnimation* ai_anim = ai_scene->mAnimations[i];
		
		AnimationSequence new_seq{};

		new_seq.fps = static_cast<float>(ai_anim->mTicksPerSecond);
		if (new_seq.fps <= 0.0f)
			new_seq.fps = 24.0f;
		new_seq.num_ticks = ai_anim->mDuration;
		new_seq.duration = new_seq.num_ticks / new_seq.fps;

		auto& ai_anim_map = animation_sequences.emplace(std::string{ ai_anim->mName.data }, new_seq).first->second.node_map;

		for (unsigned k = 0; k < ai_anim->mNumChannels; ++k)
		{
			aiNodeAnim* ai_anim_node = ai_anim->mChannels[k];
			ai_anim_map.emplace(std::string{ ai_anim_node->mNodeName.data }, ai_anim_node);
		}
	}
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

void Model::VertexBoneData::addBoneData(int id, float weight)
{
	for (unsigned i = 0; i < 4; i++) 
	{
		if (weights[i] == 0.0) 
		{
			ids[i] = id;
			weights[i] = weight;
			return;
		}
	}
}
