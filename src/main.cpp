#include <iostream>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <openglDebug.h>
#include <stb_image/stb_image.h>

// Our stuff
#include "shader_s.h"


#define USE_GPU_ENGINE 0
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}

class RandomShapes
{
public:
    RandomShapes()
        : shaderBlue(RESOURCES_PATH "shaders/blue.shader"), shaderGreen(RESOURCES_PATH "shaders/green.shader")
    {
        // First set of triangles (top right rectangle and bottom left triangle)

        // A vertex contains a bunch of data about a point *including* its position
        // It can also have uv map coords, normals, colors, etc. 
        // These are just the positions aka coordinates.
        float positions[] = {
            // Rectangle           
            // Positions         // Colors
            0.75f,  0.75f, 0.0f, 1.0f, 0.0f, 0.0f,  // top right
            0.75f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // bottom right
            0.0f,   0.0f,  0.0f, 0.0f, 0.0f, 1.0f, // bottom left
            0.0f,   0.75f, 0.0f, 0.0f, 1.0f, 0.0f, // top left

            // Triangle
             0.0f,   -0.75f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -0.75f,   0.0f,  0.0f, 0.25f, 0.0f, 0.5f  // top left
        };
        unsigned int indices[] = {
            // rectangle
            0, 1, 3, // first triangle
            1, 2, 3, // second triangle
            // third triangle
            2, 4, 5
        };

        // A buffer in OpenGL is data sent to the GPU
        // This is the id of the generated buffer - everything you give OpenGL gets an id
        // You can later reference the id to let OpenGL know what you want to use
        // VBO = vertex buffer objects (stores the actual object vertices)
        // VAO = vertex array objects (stores configuration of the attributes)
        // EBO = element buffer objects (stores indices that OpenGL uses to decide what vertices to draw)
        unsigned int VBOs[2], EBOs[2];
        glGenVertexArrays(2, VAOs);
        glGenBuffers(2, VBOs);
        glGenBuffers(2, EBOs);

        // Bind the VAO first, then bind and set vertex buffers, then configure attributes
        glBindVertexArray(VAOs[0]);

        // To select a buffer in ogl, you bind it
        // ogl has many types of buffer objects, and the buffer type of a vertex buffer is GL_ARRAY_BUFFER
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
        // Once you have a buffer selected, you write your data to it - size is provided in bytes (see doc.GL)
        // Note the third arg is requires a pointer to the vertices, but pointer decay allows us to just pass the array
        // Due to pointer decay, the array automatically decays to a pointer to its first element (i.e. positions == &positions[0])
        // https://docs.gl/gl4/glBufferData
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // So far, OpenGL has just received a blob of floats in an array.
        // It doesn't have any context on what these numbers mean or what to do with them.
        // The numbers could be a texture coords or something else - we have to tell it it's 3d vertex coordinates for a triangle
        // This tells it that we are referring to the "first" (0) attribute of a vertex - and only one for now - which is the position
        // This corresponds to the `layout (location = 0)` code in our vertex shader.
        // The size is the amount of data points per vertex attribute - in this case 3 (3d coordinates)
        // Then we just tell it it's a float, we don't want it normalized by openGL (floats are already normalized)
        // Then we tell it the size of each attribute in bytes, and finally, the offset from the first component in a vertex
        // Since we only have position for now, that's 0

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
        glEnableVertexAttribArray(0);

        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void*)(sizeof(float) * 3));
        // This just enables the attribute
        glEnableVertexAttribArray(1);


        // Top left triangle

        float positions_2[] = {
              -0.25f,  0.75f,  0.0f, // top right
              -0.25f,  0.25f,  0.0f, // bottom right
              -0.75f,  0.25f,  0.0f, // bottom left
        };
        unsigned int indices_2[] = {
            0, 1, 2
        };

        // Bind the VAO first, then bind and set vertex buffers, then configure attributes
        glBindVertexArray(VAOs[1]);

        // To select a buffer in ogl, you bind it
        // ogl has many types of buffer objects, and the buffer type of a vertex buffer is GL_ARRAY_BUFFER
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        // Once you have a buffer selected, you write your data to it - size is provided in bytes (see doc.GL)
        // Note the third arg is requires a pointer to the vertices, but pointer decay allows us to just pass the array
        // Due to pointer decay, the array automatically decays to a pointer to its first element (i.e. positions == &positions[0])
        // https://docs.gl/gl4/glBufferData
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions_2), positions_2, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_2), indices_2, GL_STATIC_DRAW);

        // So far, OpenGL has just received a blob of floats in an array.
        // It doesn't have any context on what these numbers mean or what to do with them.
        // The numbers could be a texture coords or something else - we have to tell it it's 3d vertex coordinates for a triangle
        // This tells it that we are referring to the "first" (0) attribute of a vertex - and only one for now - which is the position
        // This corresponds to the `layout (location = 0)` code in our vertex shader.
        // The size is the amount of data points per vertex attribute - in this case 3 (3d coordinates)
        // Then we just tell it it's a float, we don't want it normalized by openGL (floats are already normalized)
        // Then we tell it the size of each attribute in bytes, and finally, the offset from the first component in a vertex
        // Since we only have position for now, that's 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        // This just enables the attribute
        glEnableVertexAttribArray(0);
    };

    void draw()
    {
        shaderBlue.use();
        shaderBlue.setFloat("horizontalOffset", 0.15f);

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

        shaderGreen.use();
        std::array<float, 4> vec4 = { 0.0f, greenValue, 0.0f, 1.0f };
        shaderGreen.setVec4("ourColor", vec4);

        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~RandomShapes()
    {
        std::cout << "Destructor called." << std::endl;
        shaderBlue.deleteShader();
        shaderGreen.deleteShader();
    }

private:
    Shader shaderBlue;
    Shader shaderGreen;
    unsigned int VAOs[2];

};


class ContainerTexture
{
public:
    ContainerTexture()
        : shaderContainer(RESOURCES_PATH "shaders/container.shader")
    {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // Set texture wrapping/filtering options
        // on currently bound texture object.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Load and generate the texture
        int width, height, nChannels;
        unsigned char* data = stbi_load(RESOURCES_PATH "container.jpg", &width, &height, &nChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        float vertices[] = {
            // positions          // colors           // texture coords
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
             0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
        };
        unsigned int indices[] = {
            // rectangle
            0, 1, 3, // first triangle
            1, 2, 3, // second triangle
        };

        unsigned int VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
        glEnableVertexAttribArray(0);

        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
        // This just enables the attribute
        glEnableVertexAttribArray(1);

        // texture coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
        glEnableVertexAttribArray(2);
    }

    void draw()
    {
        shaderContainer.use();

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
private:
    unsigned int VAO;
    Shader shaderContainer;
    unsigned int texture;
};


static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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


	GLFWwindow* window = window = glfwCreateWindow(800, 600, "OpenGL Experiments", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


#pragma region report opengl errors to std
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#pragma endregion

	std::cout << "Using OpenGL Version:" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;

	// Behind the scenes, this is used to transform 2d coordinates to coordinates on screen
	// E.g., (-0.5,0.5) would (as its final transformation) be mapped to (200,450) in screen coords
	glViewport(0, 0, 800, 600);
	// Listen to window resize events and update viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    //RandomShapes randomShapes;
    ContainerTexture containerTexture;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        //randomShapes.draw();
        containerTexture.draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//there is no need to call the clear function for the libraries since the os will do that for us.
	//by calling this functions we are just wasting time.
	//glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
