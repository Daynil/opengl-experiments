#include "display.h"

#include <glad/glad.h>

Display::Display(float pDisplayWidth, float pDisplayHeight, std::string title)
{
	displayWidth = pDisplayWidth;
	displayHeight = pDisplayHeight;

	window = glfwCreateWindow(displayWidth, displayHeight, title.c_str(), NULL, NULL);
	if (!window)
		return;

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// Associate this Display instance with the GLFW window
	glfwSetWindowUserPointer(window, this);

	// Behind the scenes, this is used to transform 2d coordinates to coordinates on screen
	// E.g., (-0.5,0.5) would (as its final transformation) be mapped to (200,450) in screen coords
	glViewport(0, 0, displayWidth, displayHeight);

	// Listen to window resize events and update viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

void Display::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Retrieve the instance associated with this window
	Display* display = static_cast<Display*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);
	display->displayWidth = width;
	display->displayHeight = height;
}
