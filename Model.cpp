#include "Model.h"

#include <assimp/scene.h>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "global.h"


Model::Model(const std::string Path, const std::string Name, const bool flipWindingOrder, const bool loadMaterial) 
	: m_name(Name), m_path(Path) 
{

	if (!loadModel(Path, flipWindingOrder, loadMaterial)) 
	{
		std::cerr << "Failed to load: " << Name << '\n';
	}
}

Model::Model(const std::string Name, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const MaterialPtr& material) noexcept : m_name(Name) 
{
	m_meshes.emplace_back(vertices, indices, material);
}


Model::Model(const std::string Name, const Mesh& mesh) noexcept : m_name(Name) {
	m_meshes.push_back(mesh);
}


void Model::AttachMesh(const Mesh mesh) noexcept {
	m_meshes.push_back(mesh);
}


void Model::Delete() {
	for (auto& mesh : m_meshes) {
		OPENGLFUNCTIONS->glDeleteVertexArrays(1, &mesh.VAO);
	}
}

void Model::SetMeshTexture(int meshIndex, std::string texturePath)
{
	if (texturePath.empty())
		return;

	if (meshIndex < 0 || meshIndex >= m_meshes.size())
		return;

	m_meshes.at(meshIndex).material->Init("", texturePath);
}

bool Model::ExportModel(std::string filePath, ExportFormat exportFormat)
{
    std::string format = (exportFormat == ExportFormat::OBJ) ? "obj" : "stl";
    Assimp::Exporter exporter;

    auto scene = m_importer.GetScene();
    aiReturn exportStatus = exporter.Export(m_importer.GetScene(), format, filePath);

    if (exportStatus != aiReturn_SUCCESS)
    {
        std::cerr << "ERROR::ASSIMP::Failed to export model" << std::endl;
        return false;
    }

    return true;
}

bool Model::loadModel(const std::string Path, const bool flipWindingOrder, const bool loadMaterial = true) {
#ifdef _DEBUG
	std::cout << "Loading model: " << m_name << '\n';
#endif

	std::cout << sizeof(Material) << '\n';
	
	const aiScene* pScene{ nullptr };

	if (flipWindingOrder) {
		pScene = m_importer.ReadFile(Path.data(), aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenUVCoords |
			aiProcess_SortByPType |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FindInvalidData |
			aiProcess_FlipUVs |
			aiProcess_FlipWindingOrder | // Reverse back-face culling
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_SplitLargeMeshes);
	}
	else {
		pScene = m_importer.ReadFile(Path.data(), aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_GenUVCoords |
			aiProcess_SortByPType |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_FindInvalidData |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_ImproveCacheLocality |
			aiProcess_OptimizeMeshes |
			aiProcess_SplitLargeMeshes);
	}

	// Check if scene is not null and model is done loading
	if (!pScene || pScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) 
	{
		std::cerr << "Assimp Error for " << m_name << ": " << m_importer.GetErrorString() << '\n';
		m_importer.FreeScene();

		return false;
	}

	processNode(pScene->mRootNode, pScene, loadMaterial);

	m_path = Path.substr(0, Path.find_last_of('/')); // Strip the model file name and keep the model folder.
	m_path += "/";

	//importer.FreeScene();
	return true;
}

/***********************************************************************************/
void Model::processNode(aiNode* node, const aiScene* scene, const bool loadMaterial) {

	// Process all node meshes
	for (auto i = 0; i < node->mNumMeshes; ++i) {
		auto* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh, scene, loadMaterial));
	}

	// Process their children via recursive tree traversal
	for (auto i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene, loadMaterial);
	}
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, const bool loadMaterial) 
{
	std::vector<Vertex> vertices;


	for (auto i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		if (mesh->HasPositions()) {
			vertex.Position.x = mesh->mVertices[i].x;
			vertex.Position.y = mesh->mVertices[i].y;
			vertex.Position.z = mesh->mVertices[i].z;

		}

		if (mesh->HasNormals()) {
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents()) {
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;
		}

		if (mesh->HasTextureCoords(0) && loadMaterial) {
			// Just take the first set of texture coords (since we could have up to 8)
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// Get indices from each face
	std::vector<GLuint> indices;
	for (auto i = 0; i < mesh->mNumFaces; ++i) {
		const auto face = mesh->mFaces[i];
		for (auto j = 0; j < face.mNumIndices; ++j) {
			indices.emplace_back(face.mIndices[j]);
		}
	}

	// Process material
	// http://assimp.sourceforge.net/lib_html/structai_material.html
	if (loadMaterial) {
		if (mesh->mMaterialIndex >= 0) {
			const auto* mat = scene->mMaterials[mesh->mMaterialIndex];

			aiString name;
			mat->Get(AI_MATKEY_NAME, name);

			// Is the material cached?
			/*const auto cachedMaterial = ResourceManager::GetInstance().GetMaterial(name.C_Str());
			if (cachedMaterial.has_value()) {
				return Mesh(vertices, indices, cachedMaterial.value());
			}*/

			// Get the first texture for each texture type we need
			// since there could be multiple textures per type
			aiString albedoPath;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &albedoPath);

			aiString metallicPath;
			mat->GetTexture(aiTextureType_AMBIENT, 0, &metallicPath);

			aiString normalPath;
			mat->GetTexture(/*aiTextureType_HEIGHT*/aiTextureType_DIFFUSE, 0, &normalPath);

			aiString roughnessPath;
			mat->GetTexture(aiTextureType_SHININESS, 0, &roughnessPath);

			aiString alphaMaskPath;
			mat->GetTexture(aiTextureType_OPACITY, 0, &alphaMaskPath);

			auto newMat = Material();
			newMat.Init(name.C_Str(), normalPath.C_Str());


			++m_numMats;
			return Mesh(vertices, indices, std::make_shared<Material>(newMat));
		}
	}

	return Mesh(vertices, indices);
}