#include "Mesh.h"
#include <iostream>
#include <set>
#include <vector>

Vertex::Vertex(const glm::vec3 & pos, const glm::vec3 & norm, const glm::vec3 & text)
	: position{ pos }, normal{ norm }, uv{ text }
{
}

void Mesh::Cleanup()
{
	// Cleanup buffers
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(2, normalsVBO);
}

void Mesh::setUpMesh(const std::vector<Vertex>& verts, const std::vector<unsigned> inds)
{
	// Setting up the mesh.
	vertices.clear();
	indices.clear();
	faceNormalsDisplay.clear();
	vertNormalsDisplay.clear();
	VBO = 0;
	EBO = 0;
	vertices = verts;
	indices = inds;
}

int Mesh::setUpBuffers()
{
	// Creating the EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(	GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned),
					indices.data(), GL_STATIC_DRAW);

	// Creating the VBO for vertices.
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
	
	// Creating the VBO for UVs. (POSITION)
	glGenBuffers(MAX_UV_TYPE, uvsVBO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[0][PLANAR]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[0][PLANAR].size() * sizeof(glm::vec2),
				 vertexUVs[0][PLANAR].data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[0][CYLINDRICAL]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[0][CYLINDRICAL].size() * sizeof(glm::vec2),
				 vertexUVs[0][CYLINDRICAL].data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[0][SPHERICAL]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[0][SPHERICAL].size() * sizeof(glm::vec2),
				 vertexUVs[0][SPHERICAL].data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[0][CUBE]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[0][CUBE].size() * sizeof(glm::vec2),
				 vertexUVs[0][CUBE].data(), GL_STATIC_DRAW);

	// CREATING UV VBOS FOR NORMALS
	glGenBuffers(MAX_UV_TYPE, uvsVBO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[1][PLANAR]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[1][PLANAR].size() * sizeof(glm::vec2),
				 vertexUVs[1][PLANAR].data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[1][CYLINDRICAL]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[1][CYLINDRICAL].size() * sizeof(glm::vec2),
				 vertexUVs[1][CYLINDRICAL].data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[1][SPHERICAL]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[1][SPHERICAL].size() * sizeof(glm::vec2),
				 vertexUVs[1][SPHERICAL].data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[1][CUBE]);
	glBufferData(GL_ARRAY_BUFFER, vertexUVs[1][CUBE].size() * sizeof(glm::vec2),
				 vertexUVs[1][CUBE].data(), GL_STATIC_DRAW);
	
	// BINDING
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO[0][CUBE]);
	glVertexAttribPointer(	2,
							2,
							GL_FLOAT,
							GL_FALSE,
							sizeof(glm::vec2),
							(void *)(0));
	
	/*glVertexAttribPointer(	2,
							2,
							GL_FLOAT,
							GL_FALSE,
							sizeof(Vertex),
							(void *)(sizeof(glm::vec3) * 2));*/
	glEnableVertexAttribArray(2);
	
	return 0;
}

int Mesh::setUpNormalsBuffers()
{
	// Setting up the bufffers needed
	glGenBuffers(2, normalsVBO);

	// Vertex normals VBO
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertNormalsDisplay.size() * sizeof(glm::vec3),
				 vertNormalsDisplay.data(), GL_STATIC_DRAW);
	
	// Face normals VBO
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, faceNormalsDisplay.size() * sizeof(glm::vec3),
				 faceNormalsDisplay.data(), GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0,
						  3,
						  GL_FLOAT,
						  GL_FALSE,
						  sizeof(glm::vec3),
						  (void *)(0));
	glEnableVertexAttribArray(0);
	return 0;
}

void Mesh::normalize(const glm::mat4& scaleMat)
{
	for(auto& vert : vertices)
		vert.position = scaleMat * glm::vec4{ vert.position, 1.0f };
}

glm::vec3 Mesh::getModelScale() const
{
	glm::vec3 scale = boundingBox[1] - boundingBox[0];
	if (scale.x == 0)
		scale.x = 1.0;

	if (scale.y == 0)
		scale.y = 1.0;

	if (scale.z == 0)
		scale.z = 1.0;

	return scale;
}

glm::vec3  Mesh::getModelCentroid() const
{
	return glm::vec3{ boundingBox[0] + boundingBox[1] } *0.5f;
}

struct compareVec
{
	compareVec() = default;
	bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
	{
		glm::vec3 cross_vec = glm::cross(lhs, rhs);
		if (std::fabs(cross_vec.x) < 0.001f &&
			std::fabs(cross_vec.y) < 0.001f &&
			std::fabs(cross_vec.z) < 0.001f
			)
			return false;

		if (std::fabs(lhs.x - rhs.x) > 0.001f) return lhs.x < rhs.x;
		if (std::fabs(lhs.y - rhs.y) > 0.001f) return lhs.y < rhs.y;
		if (std::fabs(lhs.z - rhs.z) > 0.001f) return lhs.z < rhs.z;
		return false;
	}
};

int Mesh::calcVertexNormals(bool flip)
{
	int rFlag = -1;

	// type 3 and 4 have normals provided
	if (vertexType > 2)
		return rFlag;
	
	// vertices and indices must be populated
	if (vertices.empty() || indices.empty())
	{
		std::cout << "Cannot calculate vertex normals for empty mesh." << std::endl;
		return rFlag;
	}
	
	// This should be a pair of vertPos and vertNormal
	vertNormalsDisplay.resize(vertices.size() * 2, glm::vec3{ 0.0, 0.0, 0.0 });
	// NumFaces = indices/3.
	// NumTotalData = NumFaces * 2
	faceNormalsDisplay.resize((indices.size()/3) * 2, glm::vec3{ 0.0, 0.0, 0.0 });
	std::vector< std::set< glm::vec3, compareVec> >  vNormalSet;
	vNormalSet.resize(vertices.size(), std::set<glm::vec3, compareVec>{});
	const float normalLength = 0.05f;
	// For every face
	for (unsigned index = 0; index < indices.size(); )
	{
		GLuint a = indices.at(index++);
		GLuint b = indices.at(index++);
		GLuint c = indices.at(index++);

		glm::vec3  vA = vertices[a].position;
		glm::vec3  vB = vertices[b].position;
		glm::vec3  vC = vertices[c].position;

		glm::vec3 faceCenter = (vA + vB + vC) / 3.0f;

		// Edge vectors
		glm::vec3  E1 = vB - vA;
		glm::vec3  E2 = vC - vA;

		glm::vec3  N = glm::normalize(glm::cross(E1, E2));

		if (flip)
			N = -N;
		// Check for duplicates

		// For vertex a
		vNormalSet.at(a).insert(N);
		vNormalSet.at(b).insert(N);
		vNormalSet.at(c).insert(N);

		faceNormalsDisplay.push_back(faceCenter);
		faceNormalsDisplay.push_back(faceCenter + (N * normalLength));
	}

	// Now sum up the values per vertex
	for (int index = 0; index < vNormalSet.size(); ++index)
	{
		glm::vec3  vNormal(0.0f);

		
		auto nIt = vNormalSet[index].begin();
		while (nIt != vNormalSet[index].end())
		{
			vNormal += (*nIt);
			++nIt;
		}

		// save vertex normal
		vertices[index].normal = glm::normalize(vNormal);

		// save normal to display
		glm::vec3  vA = vertices[index].position;

		vertNormalsDisplay[2 * index] = vA;
		vertNormalsDisplay[(2 * index) + 1] = vA + vertices[index].normal * normalLength;

	}

	// success
	rFlag = 0;

	return rFlag;
}


glm::vec2 convert_xyz_to_cube_uv(glm::vec3 vertex)
{
	auto floatCompare = [](float a, float b)
	{
		return std::fabs(a - b) < 0.001f;
	};

	float absX = fabs(vertex.x);
	float absY = fabs(vertex.y);
	float absZ = fabs(vertex.z);

	glm::vec2 uv;

	float maxAxis;
	// X AXIS
	if ((absX > absY || floatCompare(absX, absY)) && (absX >= absZ || floatCompare(absX, absZ)))
	{
		if (vertex.x < 0)
		{
			uv.x = vertex.z;
			uv.y = vertex.y;
		}
		else
		{
			uv.x = -vertex.z;
			uv.y = vertex.y;
		}

		maxAxis = absX;
	}
	// Y AXIS
	else if ((absY > absX || floatCompare(absY, absX)) && (absY >= absZ || floatCompare(absY, absZ)))
	{
		if (vertex.y < 0)
		{
			uv.x = vertex.x;
			uv.y = vertex.z;
		}
		else
		{
			uv.x = vertex.x;
			uv.y = -vertex.z;
		}

		maxAxis = absY;
	}
	// Z AXIS
	else if ((absZ > absX || floatCompare(absZ, absX)) && (absZ >= absY || floatCompare(absZ, absY)))
	{
		if (vertex.z < 0)
		{
			uv.x = -vertex.x;
			uv.y = vertex.y;
		}
		else
		{
			uv.x = vertex.x;
			uv.y = vertex.y;
		}

		maxAxis = absZ;
	}

	// Convert range from -1 to 1 to 0 to 1
	uv.x = 0.5f * (uv.x / maxAxis + 1.0f);
	uv.y = 0.5f * (uv.y / maxAxis + 1.0f);

	return uv;
}

int Mesh::calcVertexUVs()
{
	int rFlag = -1;
	
	glm::vec3 delta = getModelScale();
	glm::vec3 center = getModelCentroid();

	for (unsigned i = 0; i < MAX_UV_TYPE; ++i)
	{
		vertexUVs[0][i].clear();
		vertexUVs[0][i].reserve(vertices.size());

		vertexUVs[1][i].clear();
		vertexUVs[1][i].reserve(vertices.size());
	}
	

	for (int nVertex = 0; nVertex < vertices.size(); ++nVertex)
	{
		glm::vec3 V = vertices[nVertex].position;
		glm::vec3 N = vertices[nVertex].normal;
		
		// PLANAR
		{
			glm::vec3 normVertex = ((V - center) + 1.0f) / delta;
			glm::vec2 uv{ normVertex.x, normVertex.y };
			vertexUVs[POSITION][PLANAR].emplace_back(uv);

			float max = std::fmaxf(abs(N.x), abs(N.y));
			max = std::fmaxf(max, abs(N.z));
			glm::vec3 normNormal = N / (2.0f * max) + 0.5f;
			vertexUVs[NORMAL][PLANAR].emplace_back(normNormal.x, normNormal.y);
		}								  

		// CYLINDRICAL
		{
			float theta(0.0f);
			float z(0.0f);
			float phi(0.0f);

			theta = glm::degrees(static_cast<float>(atan2(V.y, V.x)));
			z = V.z;
			vertexUVs[POSITION][CYLINDRICAL].emplace_back(theta / 360.0f, z);

			theta = glm::degrees(static_cast<float>(atan2(N.y, N.x)));
			z = N.z;
			vertexUVs[NORMAL][CYLINDRICAL].emplace_back(theta / 360.0f, z);
		}
		
		// SPHERICAL
		{
			float theta(0.0f);
			float z(0.0f);
			float phi(0.0f);

			theta = glm::degrees(glm::atan(V.y, V.x));
			z = V.z;
			phi = glm::degrees(glm::acos(z / V.length()));
			vertexUVs[POSITION][SPHERICAL].emplace_back(theta / 360.0f, 1.0f - (phi / 180.0f));

			theta = glm::degrees(glm::atan(N.y, N.x));
			z = N.z;
			phi = glm::degrees(glm::acos(z / N.length()));
			vertexUVs[NORMAL][SPHERICAL].emplace_back(theta / 360.0f, 1.0f - (phi / 180.0f));
		}
		
		// CUBE MAP
		{
			glm::vec2 uv = convert_xyz_to_cube_uv(V);
			vertexUVs[POSITION][CUBE].emplace_back(uv);

			uv = convert_xyz_to_cube_uv(N);
			vertexUVs[NORMAL][CUBE].emplace_back(uv);
		}
	}

	return 0;
}

