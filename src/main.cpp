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


#define USE_GPU_ENGINE 0
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}


struct GameState
{
    glm::vec3 pos;
    glm::vec3 move;
    float speed;
    float fps;
    float deltaTime;

    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;

    float lastX;
    float lastY;

    float yaw;
    float pitch;

    float fov;

    int joystick_id;
};

struct GameState gameState;

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
        shaderBlue.activate();
        shaderBlue.setFloat("horizontalOffset", 0.15f);

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

        shaderGreen.activate();
        glm::vec4 color = { 0.0f, greenValue, 0.0f, 1.0f };
        shaderGreen.setVec4("ourColor", color);

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
    Shader shaderContainer;

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
        shaderContainer.activate();

        glm::mat4 trans = glm::mat4(1.0f);

        // Static example
        //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        //trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        

        // Over time example (bounces around)
        //float time = (float)glfwGetTime();
        //float clamped_time = std::fmod(time, 1);

        //if (((int)time) % 2 == 0)
        //{
        //    clamped_time = 1 - clamped_time;
        //}
        //float x_translate = ((2 * clamped_time) - 1);

        //trans = glm::translate(trans, glm::vec3(x_translate, x_translate, 0.0f));
        //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        // Using input
        //trans = glm::translate(trans, gameState.move);
        //shaderContainer.setMat4("transform", glm::value_ptr(trans));


        // 3D
        // Model transforms from local coords to world coords
        // Here we rotate -55 degrees along the x axis to simulate laying on a plane
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        // The view transforms from world coords to view coords
        // Here we set the camera a few units away from the scene (aka, shift the whole scene back)
        // Note OGL uses a right-handed system, so -z is backward.
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // This creates the projection
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        shaderContainer.setMat4("model", glm::value_ptr(model));
        shaderContainer.setMat4("view", glm::value_ptr(view));
        shaderContainer.setMat4("projection", glm::value_ptr(projection));

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
private:
    unsigned int VAO;
    unsigned int texture;
};

class Cubes
{
public:
    Shader shaderContainer;

    Cubes()
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
            // positions          // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
        glEnableVertexAttribArray(0);

        // texture coords
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
        glEnableVertexAttribArray(1);
    }

    void draw()
    {
        shaderContainer.activate();

        glm::mat4 trans = glm::mat4(1.0f);

        // Static example
        //trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        //trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        

        // Over time example (bounces around)
        //float time = (float)glfwGetTime();
        //float clamped_time = std::fmod(time, 1);

        //if (((int)time) % 2 == 0)
        //{
        //    clamped_time = 1 - clamped_time;
        //}
        //float x_translate = ((2 * clamped_time) - 1);

        //trans = glm::translate(trans, glm::vec3(x_translate, x_translate, 0.0f));
        //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        //trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        // Using input
        //trans = glm::translate(trans, gameState.move);
        //shaderContainer.setMat4("transform", glm::value_ptr(trans));


        // 3D
        // Model transforms from local coords to world coords
        // Here we rotate -55 degrees along the x axis to simulate laying on a plane
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // The view transforms from world coords to view coords
        // Here we set the camera a few units away from the scene (aka, shift the whole scene back)
        // Note OGL uses a right-handed system, so -z is backward.
        //glm::mat4 view = glm::mat4(1.0f);
        //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // Camera style with the lookAt function, rotates over time
        //const float radius = 10.0f;
        //float camX = sin(glfwGetTime()) * radius;
        //float camZ = cos(glfwGetTime()) * radius;
        //glm::mat4 view;
        //view = glm::lookAt(
        //    // Position (of camera in world space) vector
        //    glm::vec3(camX, 0.0f, camZ),
        //    // Target (of camera in world space) vector
        //    glm::vec3(0.0f, 0.0f, 0.0f),
        //    // Up vector
        //    glm::vec3(0.0f, 1.0f, 0.0f)
        //);

        // Camera looks around with keys
        glm::mat4 view;
        view = glm::lookAt(gameState.cameraPos, gameState.cameraPos + gameState.cameraFront, gameState.cameraUp);

        // This creates the projection
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(gameState.fov), 800.0f / 600.0f, 0.1f, 100.0f);

        shaderContainer.setMat4("view", glm::value_ptr(view));
        shaderContainer.setMat4("projection", glm::value_ptr(projection));

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        
        glm::vec3 cubePositions[] = {
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

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * (i + 1);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shaderContainer.setMat4("model", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }



        glBindVertexArray(0);
    }
private:
    unsigned int VAO;
    unsigned int texture;
};


//static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GLFW_TRUE);
//
//    if ((key == GLFW_KEY_UP || key == GLFW_KEY_W) && action == GLFW_PRESS)
//        gameState.cameraPos += gameState.speed * gameState.deltaTime * gameState.cameraFront;
//    if ((key == GLFW_KEY_LEFT || key == GLFW_KEY_A) && action == GLFW_PRESS)
//        gameState.cameraPos -= glm::normalize(glm::cross(gameState.cameraFront, gameState.cameraUp)) * gameState.speed * gameState.deltaTime;
//    if ((key == GLFW_KEY_DOWN || key == GLFW_KEY_S) && action == GLFW_PRESS)
//        gameState.cameraPos -= gameState.speed * gameState.deltaTime * gameState.cameraFront;
//    if ((key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) && action == GLFW_PRESS)
//        gameState.cameraPos += glm::normalize(glm::cross(gameState.cameraFront, gameState.cameraUp)) * gameState.speed * gameState.deltaTime;
//}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        gameState.cameraPos += gameState.speed * gameState.deltaTime * gameState.cameraFront;
//    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        gameState.cameraPos -= glm::normalize(glm::cross(gameState.cameraFront, gameState.cameraUp)) * gameState.speed * gameState.deltaTime;
//    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        gameState.cameraPos -= gameState.speed * gameState.deltaTime * gameState.cameraFront;
//    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        gameState.cameraPos += glm::normalize(glm::cross(gameState.cameraFront, gameState.cameraUp)) * gameState.speed * gameState.deltaTime;
//
//    if (gameState.joystick_id >= 0)
//    {
//        GLFWgamepadstate state;
//        if (glfwGetGamepadState(gameState.joystick_id, &state))
//        {
//            float leftStickX = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
//            float leftStickY = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
//
//            // Invert y (defaults to down is up)
//            gameState.cameraPos += -leftStickY * gameState.speed * gameState.deltaTime * gameState.cameraFront;
//            gameState.cameraPos += glm::normalize(glm::cross(gameState.cameraFront, gameState.cameraUp)) * leftStickX * gameState.speed * gameState.deltaTime;
//
//            float rightStickX = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
//            float rightStickY = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
//
//            const float sensitivity = 1.0f;
//            gameState.yaw += (rightStickX * sensitivity);
//            // By default right stick uses airplane style controls (up is down) - invert
//            gameState.pitch += (-rightStickY * sensitivity);
//
//            glm::vec3 direction;
//            direction.x = cos(glm::radians(gameState.yaw)) * cos(glm::radians(gameState.pitch));
//            direction.y = sin(glm::radians(gameState.pitch));
//            direction.z = sin(glm::radians(gameState.yaw)) * cos(glm::radians(gameState.pitch));
//            gameState.cameraFront = glm::normalize(direction);
//
//            for (int i = 0; i < GLFW_GAMEPAD_BUTTON_LAST; ++i) {
//                if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
//                {
//                    std::cout << "A Button pressed!" << std::endl;
//                }
//                if (state.buttons[i] == GLFW_PRESS) {
//                    std::cout << "Button " << i << " pressed!" << std::endl;
//                }
//            }
//        }
//    }
//}
//
//
//bool firstMouse = true;
//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//    if (firstMouse)
//    {
//        gameState.lastX = xpos;
//        gameState.lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - gameState.lastX;
//    float yoffset = gameState.lastY - ypos;
//    gameState.lastX = xpos;
//    gameState.lastY = ypos;
//
//    const float sensitivity = 0.1f;
//    gameState.yaw += (xoffset * sensitivity);
//    gameState.pitch += (yoffset * sensitivity);
//
//    if (gameState.pitch > 89.0f)
//        gameState.pitch = 89.0f;
//    if (gameState.pitch < -89.0f)
//        gameState.pitch = -89.0f;
//
//    glm::vec3 direction;
//    direction.x = cos(glm::radians(gameState.yaw)) * cos(glm::radians(gameState.pitch));
//    direction.y = sin(glm::radians(gameState.pitch));
//    direction.z = sin(glm::radians(gameState.yaw)) * cos(glm::radians(gameState.pitch));
//    gameState.cameraFront = glm::normalize(direction);
//}
//
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    gameState.fov -= (float)yoffset;
//    if (gameState.fov < 1.0f)
//        gameState.fov = 1.0f;
//    if (gameState.fov > 45.0f)
//        gameState.fov = 45.0f;
//    std::cout << gameState.fov << std::endl;
//}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//void joystick_callback(int jid, int event)
//{
//    if (event == GLFW_CONNECTED)
//    {
//        if (glfwJoystickIsGamepad(jid))
//        {
//            std::cout << "Gamepad connected: " + (std::string)glfwGetGamepadName(jid) + " (id: " <<  jid << ")" << std::endl;
//            GLFWgamepadstate state;
//            if (glfwGetGamepadState(jid, &state))
//            {
//                gameState.joystick_id = jid;
//            }
//            else
//            {
//                std::cout << "Gamepad has no valid mapping: " + (std::string)glfwGetGamepadName(jid) << std::endl;
//            }
//        }
//
//    }
//    else if (event == GLFW_DISCONNECTED)
//    {
//        //std::cout << "Gamepad disconnected: " + (std::string)glfwGetGamepadName(jid) << std::endl;
//        std::cout << "Gamepad id " << jid << " disconnected!" << std::endl;
//        gameState.joystick_id = -1;
//    }
//}

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


	//GLFWwindow* window = window = glfwCreateWindow(800, 600, "OpenGL Experiments", NULL, NULL);
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
    Renderer renderer(shader, 800, 600);

    //const std::vector<float> vertices = {
    //        0.5f,  0.5f, 0.0f,
    //        0.5f, -0.5f, 0.0f,
    //        -0.5f, -0.5f, 0.0f,
    //        -0.5f,  0.5f, 0.0f,
    //};
    //
    //const std::vector<float> textureCoords = {
    //        1.0f, 1.0f,
    //        1.0f, 0.0f,
    //        0.0f, 0.0f,
    //        0.0f, 1.0f
    //};
    //
    //const std::vector<unsigned int> indices = {
    //    // rectangle
    //    0, 1, 3, // first triangle
    //    1, 2, 3, // second triangle
    //};

    const std::vector<float> vertices = {
        -0.5f,0.5f,0,
        -0.5f,-0.5f,0,
        0.5f,-0.5f,0,
        0.5f,0.5f,0,

        -0.5f,0.5f,1,
        -0.5f,-0.5f,1,
        0.5f,-0.5f,1,
        0.5f,0.5f,1,

        0.5f,0.5f,0,
        0.5f,-0.5f,0,
        0.5f,-0.5f,1,
        0.5f,0.5f,1,

        -0.5f,0.5f,0,
        -0.5f,-0.5f,0,
        -0.5f,-0.5f,1,
        -0.5f,0.5f,1,

        -0.5f,0.5f,1,
        -0.5f,0.5f,0,
        0.5f,0.5f,0,
        0.5f,0.5f,1,

        -0.5f,-0.5f,1,
        -0.5f,-0.5f,0,
        0.5f,-0.5f,0,
        0.5f,-0.5f,1
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


    Entity entity(RESOURCES_PATH "container.jpg", vertices, textureCoords, indices, glm::vec3(0, 0, 0));

	//glfwSetKeyCallback(window, key_callback);

    // Disable cursor for best FPS mode, removed for testing
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    /*glfwSetCursorPosCallback(display.window, mouse_callback);
    glfwSetScrollCallback(display.window, scroll_callback);

    glfwSetJoystickCallback(joystick_callback);*/

	/*glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);*/




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
	//glViewport(0, 0, 800, 600);
	// Listen to window resize events and update viewport
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    //gameState.move = glm::vec3(0.0f, 0.0f, 0.0f);
    //gameState.speed = 2.5f;

    //gameState.cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    //gameState.cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    //gameState.cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    //gameState.lastX = 400;
    //gameState.lastY = 300;
    //
    //gameState.yaw = 0;
    //gameState.pitch = 0;

    //gameState.fov = 45.0f;

    //gameState.joystick_id = -1;
    //for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_16; i++)
    //{
    //    if (glfwJoystickPresent(i))
    //        joystick_callback(i, GLFW_CONNECTED);
    //}

    //RandomShapes randomShapes;
    //ContainerTexture containerTexture;
    //Cubes cubes;

    //glEnable(GL_DEPTH_TEST);

    float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(display.window))
	{
        //processInput(display.window);
        renderer.prepare();
        renderer.render(entity, shader, camera);

		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        // Delta time = seconds per frame (s/f)
        gameState.deltaTime = currentFrame - lastFrame;
        gameState.fps = 1 / gameState.deltaTime;

        lastFrame = currentFrame;

        //std::cout << gameState.fps << " " << gameState.deltaTime << std::endl;
        
        //randomShapes.draw();
        //containerTexture.draw();
        //cubes.draw();

		glfwSwapBuffers(display.window);
		glfwPollEvents();
	}

	//there is no need to call the clear function for the libraries since the os will do that for us.
	//by calling this functions we are just wasting time.
	//glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
