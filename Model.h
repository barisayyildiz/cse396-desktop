#pragma once

#include <glm/mat4x4.hpp>

#include "Mesh.h"

#include <memory>
#include <string>
#include <string_view>

#include <assimp/Importer.hpp>

enum ExportFormat {
    OBJ,
    STL
};


struct aiScene;
struct aiNode;
struct aiMesh;

class Model {
public:
	Model() = default;
	Model(const std::string Path, const std::string Name, const bool flipWindingOrder = false, const bool loadMaterial = true);
	Model(const std::string Name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const MaterialPtr& material) noexcept;
	Model(const std::string Name, const Mesh& mesh) noexcept;
	virtual ~Model() = default;

	void AttachMesh(const Mesh mesh) noexcept;

	// Destroys all OpenGL handles for all submeshes. This should only be called by ResourceManager.
	void Delete();

	auto GetMeshes() const noexcept { return m_meshes; }

	void SetMeshTexture(int meshIndex, std::string texturePath);

    bool ExportModel(std::string filePath, ExportFormat exportFormat);

protected:
	std::vector<Mesh> m_meshes;

private:
	bool loadModel(const std::string Path, const bool flipWindingOrder, const bool loadMaterial);
	void processNode(aiNode* node, const aiScene* scene, const bool loadMaterial);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, const bool loadMaterial);

	Assimp::Importer m_importer;

	// Model name
	const std::string m_name;
	// Location on disk holding model and textures
	std::string m_path;

	std::size_t m_numMats;
};

using ModelPtr = std::shared_ptr<Model>;
