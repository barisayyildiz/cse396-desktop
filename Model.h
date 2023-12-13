#ifndef H_MODEL
#define H_MODEL

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
    Model(const std::string Path, const std::string Name);

	void AttachMesh(const Mesh mesh) noexcept;

	// Destroys all OpenGL handles for all submeshes. This should only be called by ResourceManager.
	void Delete();

	auto GetMeshes() const noexcept { return m_meshes; }

	void SetMeshTexture(int meshIndex, std::string texturePath);

    bool ExportModel(std::string filePath, ExportFormat exportFormat);

    int getTotalNumberOfMeshes() { return totalNumOfMeshes; }
    int getTotalNumberOfFaces() { return totalNumOfFaces; }
    int getTotalNumberOfNodes() { return totalNumOfNodes; }

protected:
	std::vector<Mesh> m_meshes;

private:
    bool loadModel(const std::string Path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	Assimp::Importer m_importer;

	// Model name
	const std::string m_name;
	// Location on disk holding model and textures
	std::string m_path;

	std::size_t m_numMats;

    u_int totalNumOfMeshes = 0;
    u_int totalNumOfFaces = 0;
    u_int totalNumOfNodes = 0;
};

using ModelPtr = std::shared_ptr<Model>;

#endif
