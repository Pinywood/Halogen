#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "VectorMath.h"

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;

    float MouseSensitivity;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
        : m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), MouseSensitivity(SENSITIVITY)
    {
        m_Position = position;
        m_WorldUp = up;
        m_Yaw = yaw;
        m_Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat3 GetViewMatrix() const;
    void Move(const float& deltaX, const float& deltaY, const float& deltaZ);
    void Turn(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void SetOrientation(const float& yaw, const float& pitch);
    void SetYaw(const float& yaw);
    void SetPitch(const float& pitch);

private:
    void updateCameraVectors();
};