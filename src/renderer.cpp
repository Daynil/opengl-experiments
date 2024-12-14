#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_s.h"

Renderer::Renderer(Shader& shader, float displayWidth, float displayHeight)
{
	float aspectRatio = displayWidth / displayHeight;
	projection = glm::perspective(glm::radians(FOV), aspectRatio, NEAR_PLANE, FAR_PLANE);
	shader.activate();
	shader.setMat4("projection", glm::value_ptr(projection));
	shader.deactivate();
}

void Renderer::render(Entity& entity, Shader& shader, Camera& camera)
{
	glBindVertexArray(entity.VAO_ID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	shader.activate();

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, entity.position);
	transform = glm::rotate(transform, glm::radians(entity.rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(entity.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(entity.rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(entity.scale, entity.scale, entity.scale));
	shader.setMat4("transform", glm::value_ptr(transform));

	glm::mat4 view;
	view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
	shader.setMat4("view", glm::value_ptr(view));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, entity.texture.textureID);

	// TODO: how to get this from model?
	int modelVertexCount = 36;
	glDrawElements(GL_TRIANGLES, modelVertexCount, GL_UNSIGNED_INT, 0);

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Renderer::prepare()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
