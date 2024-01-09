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

	void Set(unsigned int id);

    auto operator==(const Material& mat) const noexcept {
        return Name == mat.Name;
	}

	std::string Name;

    unsigned int GetMaterialTexture();

	static unsigned int LoadTexture(char const* path);

private:
    int materialTexture;
};

using MaterialPtr = std::shared_ptr<Material>;
