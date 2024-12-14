#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "entity.h"
#include "camera.h"

class Renderer
{
public:
	float FOV = 45.0f;
	float NEAR_PLANE = 0.1f;
	float FAR_PLANE = 100.0f;
	glm::mat4 projection;

	Renderer(Shader& shader, float displayWidth, float displayHeight);

	void render(Entity& entity, Shader& shader, Camera& camera);
	void prepare();
};