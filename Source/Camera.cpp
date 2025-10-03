#include "Camera.h"

glm::mat3 Camera::GetViewMatrix() const
{
    glm::mat3 View = glm::mat3(m_Right, m_Up, -m_Front);
    View = glm::transpose(View);
    return View;
}

void Camera::Move(const float& deltaX, const float& deltaY, const float& deltaZ)
{
    m_Position += deltaX * m_Right;
    m_Position += deltaY * m_Up;
    m_Position -= deltaZ * m_Front;
}

void Camera::Turn(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::SetOrientation(const float& yaw, const float& pitch)
{
    m_Yaw = yaw;
    m_Pitch = pitch;

    updateCameraVectors();
}

void Camera::SetYaw(const float& yaw)
{
    m_Yaw = yaw;
    updateCameraVectors();
}

void Camera::SetPitch(const float& pitch)
{
    m_Pitch = pitch;
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
