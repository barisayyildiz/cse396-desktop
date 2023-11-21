#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

layout (std140, binding = 0) uniform Matrices {
  mat4 projection;
  mat4 view;
};
uniform mat4 modelMatrix;

out vec2 TexCoords;

void main() 
{
    TexCoords = texCoords;

    vec3 FragPos = vec3(modelMatrix * vec4(position, 1.0));

    gl_Position = projection * view * vec4(FragPos, 1.0);
}