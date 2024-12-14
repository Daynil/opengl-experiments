#include "entity.h"

#include <glm/glm.hpp>


Entity::Entity(std::string texturePath, const std::vector<float>& vertex_positions, const std::vector<float>& vertex_texture_uvs, const std::vector<unsigned int >& vertex_indices, glm::vec3 world_position)
	: texture(texturePath)
{
    // Create VAO to store our data in
    glGenVertexArrays(1, &VAO_ID);
    glBindVertexArray(VAO_ID);

    // Create EBO for our indices
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_indices.size() * sizeof(unsigned int), vertex_indices.data(), GL_STATIC_DRAW);

    // Create VBO for positions and textures
    unsigned int VBOs[2];
    glGenBuffers(2, VBOs);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, vertex_positions.size() * sizeof(float), vertex_positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
    glEnableVertexAttribArray(0);

    // Texture coords
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, vertex_texture_uvs.size() * sizeof(float), vertex_texture_uvs.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(1);

    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    position = world_position;
    rotationX, rotationY, rotationZ = 0;
    scale = 1;
}
