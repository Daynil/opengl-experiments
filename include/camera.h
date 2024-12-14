#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

	Camera();

    void adjustFront(float yaw, float pitch, float roll);
};