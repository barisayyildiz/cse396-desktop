#ifndef H_MESH
#define H_MESH

#include "Material.h"

#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;
	glm::vec3 Tangent;
};

class Mesh {
public:

	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const MaterialPtr& material);

	const std::size_t IndexCount;
	unsigned int VAO;
    MaterialPtr material;

private:
	void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};

#endif
