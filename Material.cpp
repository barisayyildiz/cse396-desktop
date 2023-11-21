#include "Material.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

#include "global.h"

Material::Material() {
	// Set material defaults
	std::fill(m_materialTextures.begin(), m_materialTextures.end(), 0);
	std::fill(m_materialColors.begin(), m_materialColors.end(), glm::vec3(0.0f));
}

unsigned int Material::LoadTexture(char const* path)
{
	unsigned int textureID;
	OPENGLFUNCTIONS->glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		OPENGLFUNCTIONS->glBindTexture(GL_TEXTURE_2D, textureID);
		OPENGLFUNCTIONS->glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		OPENGLFUNCTIONS->glGenerateMipmap(GL_TEXTURE_2D);

		OPENGLFUNCTIONS->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		OPENGLFUNCTIONS->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		OPENGLFUNCTIONS->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		OPENGLFUNCTIONS->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Material::Init(const std::string name, const std::string normalPath)
{
	Name = name;

	m_materialTextures[NORMAL] = LoadTexture(normalPath.c_str());
}


void Material::Init(const std::string name,
	const glm::vec3& normal,
	const float alpha)
{
	Name = name;

	m_materialColors[NORMAL] = normal;

	m_alpha = alpha;
}

void Material::Set(unsigned int id)
{
	m_materialTextures[NORMAL] = id;
}


unsigned int Material::GetParameterTexture(const ParameterType parameter) const noexcept {
	return m_materialTextures[parameter];
}

glm::vec3 Material::GetParameterColor(const ParameterType parameter) const noexcept {
	return m_materialColors[parameter];
}
