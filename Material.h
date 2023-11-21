#pragma once

#include <glm/vec3.hpp>

#include <memory>
#include <array>
#include <string>

// Material for a PBR pipeline
class Material {
public:
	Material();

	enum ParameterType {
		NORMAL,
	};

	void Init(const std::string name,
		const std::string normalPath);

	void Init(const std::string name,
		const glm::vec3& normal,
		const float alpha = 1.0f);

	void Set(unsigned int id);

	auto operator==(const Material& rhs) const noexcept {
		return Name == rhs.Name;
	}

	std::string Name;

	unsigned int GetParameterTexture(const ParameterType parameter) const noexcept;
	glm::vec3 GetParameterColor(const ParameterType parameter) const noexcept;

	auto GetAlphaValue() const noexcept {
		return m_alpha;
	}

	auto GetAlphaMask() const noexcept {
		return m_alphaMaskTexture;
	}

	static unsigned int LoadTexture(char const* path);

private:
	// unsigned int HeightMap;

	std::array<unsigned int, 5> m_materialTextures;
	std::array<glm::vec3, 5> m_materialColors;

	float m_alpha;
	unsigned int m_alphaMaskTexture;
};

using MaterialPtr = std::shared_ptr<Material>;