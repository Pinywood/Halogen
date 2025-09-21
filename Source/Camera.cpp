#include "Camera.h"

glm::mat3 Camera::GetViewMatrix() const
{
    glm::mat3 View = glm::mat3(Right, Up, -Front);
    View = glm::transpose(View);
    return View;
}

void Camera::Move(const float& deltaX, const float& deltaY, const float& deltaZ)
{
    Position += deltaX * Right;
    Position += deltaY * Up;
    Position -= deltaZ * Front;
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
