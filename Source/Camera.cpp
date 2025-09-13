#include "Camera.h"

glm::mat3 Camera::GetViewMatrix()
{
    glm::mat3 View = glm::mat3(Right, Up, -Front);
    View = glm::transpose(View);
    return View;
}

void Camera::Move(GLFWwindow* window, float distance)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Position += distance * Front;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Position -= Right * distance;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        Position -= distance * Front;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Position += Right * distance;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        Position += distance * Up;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        Position -= distance * Up;
    }
}

void Camera::Turn(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
