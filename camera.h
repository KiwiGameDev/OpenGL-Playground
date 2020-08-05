#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

static const float YAW = -90.0f;
static const float PITCH = 0.0f;
static const float SPEED = 2.5f;
static const float SENSITIVITY = 0.1f;
static const float DEFAULT_FOV = 45.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float FOV;
    float AspectRatio;

    Camera(float aspectRatio, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        FOV(DEFAULT_FOV),
        AspectRatio(aspectRatio)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    Camera(float aspectRatio, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        FOV(DEFAULT_FOV),
        AspectRatio(aspectRatio)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    
    glm::mat4 getPerspectiveMatrix()
    {
        return glm::perspective(glm::radians(FOV), AspectRatio, 0.1f, 100.0f);
    }

    void processKeyboard(CameraMovement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == CameraMovement::FORWARD)
            Position += Front * velocity;
        else if (direction == CameraMovement::BACKWARD)
            Position -= Front * velocity;
        else if (direction == CameraMovement::LEFT)
            Position -= Right * velocity;
        else if (direction == CameraMovement::RIGHT)
            Position += Right * velocity;
    }

    void processMouseMovement(float xoffset, float yoffset, bool clampPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (clampPitch)
        {
            if (Pitch > 80.0f)
                Pitch = 80.0f;
            else if (Pitch < -80.0f)
                Pitch = -80.0f;
        }

        updateCameraVectors();
    }

    void processMouseScroll(float yoffset)
    {
        FOV -= (float)yoffset;
        if (FOV < 1.0f)
            FOV = 1.0f;
        if (FOV > 45.0f)
            FOV = 45.0f;
    }

private:
    void updateCameraVectors()
    {
        float radYaw = glm::radians(Yaw);
        float radPitch = glm::radians(Pitch);
        float cosYaw = cos(radYaw);
        float sinYaw = sin(radYaw);
        float cosPitch = cos(radPitch);
        float sinPitch = sin(radPitch);

        glm::vec3 front;
        front.x = cosYaw * cosPitch;
        front.y = sinPitch;
        front.z = sinYaw * cosPitch;

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
