#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"

class Controls
{
public:
	Controls(GLFWwindow* window, Camera* camera);

	void processInput();
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(double xoffset, double yoffset);
	void joystick_callback(int jid, int event);

private:
	GLFWwindow* window;
	Camera* camera;

	float lastX;
	float lastY;
	bool firstMouse;
	int joystick_id = -1;

	float yaw;
	float pitch;
	float roll;

	//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
};