#include "Mesh.h"

#include "global.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) : IndexCount(indices.size()) {

	setupMesh(vertices, indices);
}


Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const MaterialPtr& material) :
	IndexCount(indices.size()),
	material(material) {

	setupMesh(vertices, indices);
}

void Mesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{

	OPENGLFUNCTIONS->glGenVertexArrays(1, &VAO);

	OPENGLFUNCTIONS->glBindVertexArray(VAO);

	GLuint buffer;
	OPENGLFUNCTIONS->glGenBuffers(1, &buffer);

	OPENGLFUNCTIONS->glBindBuffer(GL_ARRAY_BUFFER, buffer);
	OPENGLFUNCTIONS->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	OPENGLFUNCTIONS->glGenBuffers(1, &buffer);

	OPENGLFUNCTIONS->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	OPENGLFUNCTIONS->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);


	// Vertex Attributes

	const static auto vertexSize = sizeof(Vertex);

	OPENGLFUNCTIONS->glEnableVertexAttribArray(0);
	OPENGLFUNCTIONS->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, nullptr);

	OPENGLFUNCTIONS->glEnableVertexAttribArray(1);
	OPENGLFUNCTIONS->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

	OPENGLFUNCTIONS->glEnableVertexAttribArray(2);
	OPENGLFUNCTIONS->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, Normal)));
}
