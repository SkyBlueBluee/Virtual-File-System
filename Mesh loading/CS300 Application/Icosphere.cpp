#include "Icosphere.h"
#include "GraphicsIncludes.h"
#include <unordered_map>
#include <set>
static constexpr unsigned SUBDIVISIONS = 3;

void Icosphere::computeIcosahedron()
{
	std::vector<Vertex> icosahedron;
	std::vector<unsigned> icosahedronIndices;
	icosahedron.reserve(12);
	icosahedronIndices.reserve(12 * 3);

	// Assume radius to be 1.0f. This means the icohedron will be within 1 to -1.
	// We compute 5 vertices on the top and bottom row each.
	// Each vertex is spaced 72 degrees apart on the xz plane (z is up).
	// Top plane starts rotating in the xz plane at 90 - 72 = 18 degrees.
	// Bottom plane starts rotating in the xz plane at 270 + 72 = 342 degrees;
	float topAngle = glm::radians(18.0f);
	float bottomAngle = glm::radians(342.0f);
	const float elevation = atanf(0.5f);	// This was found online.
	const float angleOffset = glm::radians(72.0f);

	// Each vertex at +- elevation can be found by:
	// x = r*cos(elevation)*cos(72*i) 
	// y = r*cos(elevation)*sin(72*i) 
	// z = r*sin(elevation)
	const float cosEle = cosf(elevation);
	const float sinEle = sinf(elevation);

	// Add top vertex first
	icosahedron.push_back(glm::vec3{ 0, 0, 1});

	for (unsigned i = 0; i < 5; ++i)
	{
		// Calculating the top part of the icosahedron
		float xTop = cosEle * cosf(topAngle);
		float yTop = cosEle * sinf(topAngle);
		
		// Calculating the bottom part of the icosahedron
		float xBot = cosEle * cosf(bottomAngle);
		float yBot = cosEle * sinf(bottomAngle);

		// For the z coordinate of top/bottom, they are simply negations of each other.
		icosahedron.push_back(glm::vec3{ xTop, yTop, sinEle });
		icosahedron.push_back(glm::vec3{ xBot, yBot, -sinEle });

		// Offsetting the angle for the next loop
		topAngle += angleOffset;
		bottomAngle += angleOffset;
	}

	// Add bottom vertex
	icosahedron.push_back(glm::vec3{ 0, 0, -1 });

	// Now calculate the indices
	// Icohedrop is made up of 3 rows, top, middle and bottom.
	// Calculate indices for top row
	for (unsigned i = 1; i < 9; i+=2)
	{
		icosahedronIndices.push_back(0);
		icosahedronIndices.push_back(i);
		// Offset by 2 because there is a bottom vertex inbetween each top vertex
		icosahedronIndices.push_back(i + 2);
	}
	// Add the last triangle
	icosahedronIndices.push_back(0);
	icosahedronIndices.push_back(9);
	icosahedronIndices.push_back(1);
	
	// Calculate the indices in the middle
	icosahedronIndices.push_back(1); // top
	icosahedronIndices.push_back(2); // bot
	icosahedronIndices.push_back(4); // bot
	
	icosahedronIndices.push_back(1); // top
	icosahedronIndices.push_back(4); // bot
	icosahedronIndices.push_back(3); // top
	
	icosahedronIndices.push_back(3); 
	icosahedronIndices.push_back(4); 
	icosahedronIndices.push_back(6); 
	
	icosahedronIndices.push_back(3);
	icosahedronIndices.push_back(6);
	icosahedronIndices.push_back(5);
	
	icosahedronIndices.push_back(5);
	icosahedronIndices.push_back(6);
	icosahedronIndices.push_back(8);
	
	icosahedronIndices.push_back(5);
	icosahedronIndices.push_back(8);
	icosahedronIndices.push_back(7);
	
	icosahedronIndices.push_back(7);
	icosahedronIndices.push_back(8);
	icosahedronIndices.push_back(10);
	
	icosahedronIndices.push_back(7);
	icosahedronIndices.push_back(10);
	icosahedronIndices.push_back(9);
	
	icosahedronIndices.push_back(9);
	icosahedronIndices.push_back(10);
	icosahedronIndices.push_back(2);
	
	icosahedronIndices.push_back(9);
	icosahedronIndices.push_back(2);
	icosahedronIndices.push_back(1);
	
	// Calculate the indices in the middle
	for (unsigned i = 2; i < 10;)
	{
		icosahedronIndices.push_back(i);
		icosahedronIndices.push_back(11);
		icosahedronIndices.push_back(i + 2);
	
		i += 2;
	}
	// Last index
	icosahedronIndices.push_back(10);
	icosahedronIndices.push_back(11);
	icosahedronIndices.push_back(2);

	// Subdivide the icosahedron into a sphere here.
	subdivideIcosahedron(icosahedron, icosahedronIndices);

	// Mesh setup
	Mesh mesh = new Mesh{ icosahedron ,icosahedronIndices };
	
	meshes.emplace_back(mesh);

	initMeshBuffers();
}

unsigned Icosphere::addSubVert(std::vector<Vertex>& vertices, std::set<std::pair<glm::vec3, unsigned>, compareVec>& shared, const glm::vec3 & pos)
{
	// Get the index IF this vertex is unique and we pushed it into the vertices vector
	unsigned index = static_cast<unsigned>(vertices.size());

	// Try to insert into the set.
	auto result = shared.emplace(pos, index);
	// Check if the insertion failed or succeeded
	if (result.second == false)
		// If we failed, we simply point index to the element that was foudn to be the same inside the set
		index = result.first->second;	
	else
		// insertion into the set was successful. Meaning the vertex is unique and hence we push it into the vector
		vertices.emplace_back(pos);

	// Return the index
	return index;
}

void Icosphere::subdivideIcosahedron(std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
{
	// Each subdivision, we subdivide every face in the CURRENT object
	for (unsigned i = 0; i < SUBDIVISIONS; ++i)
	{
		// Since the indices are not going to be correct if we were to subdivide each face, we need to clear it.
		// Save the current indices for looping.
		std::vector<unsigned> tmpIndices{ indices };
		indices.clear();

		// Every new vertex that is created should be added to this. 
		// As 2 faces can share the same edge, we need to make sure every new half-vertex created is indeed unique. 
		std::set< std::pair<glm::vec3, unsigned>, compareVec > sharedIndices;
		
		// Subdivide every face once
		for (unsigned j = 0; j < tmpIndices.size();)
		{
			const unsigned v1Index = tmpIndices[j++];
			const unsigned v2Index = tmpIndices[j++];
			const unsigned v3Index = tmpIndices[j++];

			glm::vec3 v1 = vertices[v1Index].position;
			glm::vec3 v2 = vertices[v2Index].position;
			glm::vec3 v3 = vertices[v3Index].position;
			
			// Compute half vertex of every edge of the triangle
			// We normalize the vertex because our radius is 1.
			// addSubVert will check if the vertex is unique or not using the sharedIndices set.
			// v1 -> v2
			glm::vec3 hv1 = glm::normalize((v1 + v2) * 0.5f);
			const unsigned hv1Index = addSubVert(vertices, sharedIndices, hv1);

			// v2 -> v3
			glm::vec3 hv2 = glm::normalize((v2 + v3) * 0.5f);
			const unsigned hv2Index = addSubVert(vertices, sharedIndices, hv2);
			
			// v3 -> v1
			glm::vec3 hv3 = glm::normalize((v3 + v1) * 0.5f);
			const unsigned hv3Index = addSubVert(vertices, sharedIndices, hv3);
			
			// New Indices. Every triangle will subdivide into 4 new triangles.
			indices.push_back(v1Index);
			indices.push_back(hv1Index);
			indices.push_back(hv3Index);

			indices.push_back(hv1Index);
			indices.push_back(v2Index);
			indices.push_back(hv2Index);

			indices.push_back(hv1Index);
			indices.push_back(hv2Index);
			indices.push_back(hv3Index);

			indices.push_back(hv3Index);
			indices.push_back(hv2Index);
			indices.push_back(v3Index);
		}
	}
}