#include "controls.h"

Controls::Controls(GLFWwindow* pWindow, Camera* pCamera)
{
    window = pWindow;
    camera = pCamera;

    lastX = 400;
    lastY = 300;

    yaw = 0;
    pitch = 0;
    roll = 0;

    joystick_id = -1;

    glfwSetWindowUserPointer(window, this);

    // Disable cursor for best FPS mode, removed for testing
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
}


void Controls::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Retrieve the Display instance associated with this window
    Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(window));

    if (controls->firstMouse)
    {
        controls->lastX = xpos;
        controls->lastY = ypos;
        controls->firstMouse = false;
    }

    float xoffset = xpos - controls->lastX;
    float yoffset = controls->lastY - ypos;
    controls->lastX = xpos;
    controls->lastY = ypos;

    const float sensitivity = 0.1f;
    controls->yaw += (xoffset * sensitivity);
    controls->pitch += (yoffset * sensitivity);

    if (controls->pitch > 89.0f)
        controls->pitch = 89.0f;
    if (controls->pitch < -89.0f)
        controls->pitch = -89.0f;

    controls->camera->adjustFront(controls->yaw, controls->pitch, controls->roll);
}

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
////void mouse_callback(GLFWwindow* window, double xpos, double ypos)
////{
////    if (firstMouse)
////    {
////        gameState.lastX = xpos;
////        gameState.lastY = ypos;
////        firstMouse = false;
////    }
////
////    float xoffset = xpos - gameState.lastX;
////    float yoffset = gameState.lastY - ypos;
////    gameState.lastX = xpos;
////    gameState.lastY = ypos;
////
////    const float sensitivity = 0.1f;
////    gameState.yaw += (xoffset * sensitivity);
////    gameState.pitch += (yoffset * sensitivity);
////
////    if (gameState.pitch > 89.0f)
////        gameState.pitch = 89.0f;
////    if (gameState.pitch < -89.0f)
////        gameState.pitch = -89.0f;
////
////    glm::vec3 direction;
////    direction.x = cos(glm::radians(gameState.yaw)) * cos(glm::radians(gameState.pitch));
////    direction.y = sin(glm::radians(gameState.pitch));
////    direction.z = sin(glm::radians(gameState.yaw)) * cos(glm::radians(gameState.pitch));
////    gameState.cameraFront = glm::normalize(direction);
////}
//
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    gameState.fov -= (float)yoffset;
//    if (gameState.fov < 1.0f)
//        gameState.fov = 1.0f;
//    if (gameState.fov > 45.0f)
//        gameState.fov = 45.0f;
//    //std::cout << gameState.fov << std::endl;
//}
//
//void joystick_callback(int jid, int event)
//{
//    if (event == GLFW_CONNECTED)
//    {
//        if (glfwJoystickIsGamepad(jid))
//        {
//            std::cout << "Gamepad connected: " + (std::string)glfwGetGamepadName(jid) + " (id: " << jid << ")" << std::endl;
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