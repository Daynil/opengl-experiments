#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "shader_s.h"
#include "texture.h"

class Entity
{
public:
	unsigned int VAO_ID;
	Texture texture;

	glm::vec3 position;
	float rotationX, rotationY, rotationZ;
	float scale;

	Entity(std::string texturePath, const std::vector<float>& vertex_positions, const std::vector<float>& vertex_texture_uvs, const std::vector<unsigned int >& vertex_indices, glm::vec3 world_position);

private:
	std::vector<float> vertex_positions;
	std::vector<float> vertex_texture_uvs;
	std::vector<unsigned int> vertex_indices;
};