#pragma once
#include "Model.h"
#include <unordered_map>
#include <set>
class Icosphere : public Model
{
public:
	Icosphere() = default;
	int LoadModel() override { computeIcosahedron(); isLoaded = true; return 0; }
	int LoadModel(const std::string&) override { computeIcosahedron(); isLoaded = true; return 0; }

private:
	// This is used for checking if a vertex is unique or not. It is used when we subdivide our icosahedron.
	struct compareVec
	{
		bool operator() (const std::pair<glm::vec3, unsigned>& lhs, const std::pair<glm::vec3, unsigned>& rhs) const
		{
			glm::vec3 lhsPos = lhs.first, rhsPos = rhs.first;

			if (lhsPos.x != rhsPos.x) return lhsPos.x < rhsPos.x;
			if (lhsPos.y != rhsPos.y) return lhsPos.y < rhsPos.y;
			if (lhsPos.z != rhsPos.z) return lhsPos.z < rhsPos.z;
			return false;
		}
	};
	
	// Helper functions for generating the vertices needed for an icosphere.
	void subdivideIcosahedron(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);
	void computeIcosahedron();
	unsigned addSubVert(std::vector<Vertex>& vertices, std::set< std::pair<glm::vec3, unsigned>, compareVec >& shared, const glm::vec3& pos);
};