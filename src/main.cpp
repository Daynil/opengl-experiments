#include <iostream>
#include <cmath>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <openglDebug.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Our stuff
#include "shader_s.h"
#include "display.h"
#include "entity.h"
#include "texture.h"
#include "renderer.h"
#include "controls.h"
#include "model.h"


#define USE_GPU_ENGINE 0
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}


int main(void)
{

	if (!glfwInit())
		return -1;


#pragma region report opengl errors to std
	//enable opengl debugging output.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#pragma endregion

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //you might want to do this when testing the game for shipping

    Display display(800.0f, 600.0f, "OpenGL Experiments");
	if (!display.window)
	{
        std::cout << "***ERROR initializing glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}

    Camera camera;
    Controls controls(display.window, &camera);
    Shader shader(RESOURCES_PATH "shaders/entity.shader");
    Renderer renderer;

    const std::vector<float> vertices = {
        // Front face
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

         // Back face
         -0.5f,  0.5f,  0.5f,
         -0.5f, -0.5f,  0.5f,
          0.5f, -0.5f,  0.5f,
          0.5f,  0.5f,  0.5f,

          // Right face
           0.5f,  0.5f, -0.5f,
           0.5f, -0.5f, -0.5f,
           0.5f, -0.5f,  0.5f,
           0.5f,  0.5f,  0.5f,

           // Left face
           -0.5f,  0.5f, -0.5f,
           -0.5f, -0.5f, -0.5f,
           -0.5f, -0.5f,  0.5f,
           -0.5f,  0.5f,  0.5f,

           // Top face
           -0.5f,  0.5f,  0.5f,
           -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,

            // Bottom face
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f
    };
    
    const std::vector<float> textureCoords = {
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0,
        0,0,
        0,1,
        1,1,
        1,0
    };
    
    const std::vector<unsigned int> indices = {
        0,1,3,
        3,1,2,
        4,5,7,
        7,5,6,
        8,9,11,
        11,9,10,
        12,13,15,
        15,13,14,
        16,17,19,
        19,17,18,
        20,21,23,
        23,21,22
    };

    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    Model model(RESOURCES_PATH "container.jpg", vertices, textureCoords, indices);
    //Entity cube(&model, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.5f);

    std::vector<Entity> cubes;

    for each(glm::vec3 pos in cubePositions)
    {
        cubes.push_back(Entity(&model, pos, 45.0f, 45.0f, 0.0f, 0.5f));
    }

#pragma region report opengl errors to std
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#pragma endregion

	std::cout << "Using OpenGL Version:" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

    float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(display.window))
	{
        float currentFrame = glfwGetTime();
        // Delta time = seconds per frame (s/f)
        float deltaTime = currentFrame - lastFrame;
        float fps = 1 / deltaTime;
        lastFrame = currentFrame;

        controls.processInput(display.window, deltaTime);
        renderer.prepare();

        //renderer.render(cube, shader, camera, display);
        int idx = 0;
        for each(Entity cube in cubes)
        {
            cube.rotationZ = (float)glfwGetTime() * 20 * idx;
            renderer.render(cube, shader, camera, display);
            idx += 1;
        }

        //std::cout << gameState.fps << " " << gameState.deltaTime << std::endl;

		glfwSwapBuffers(display.window);
		glfwPollEvents();
	}

	//there is no need to call the clear function for the libraries since the os will do that for us.
	//by calling this functions we are just wasting time.
	//glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
