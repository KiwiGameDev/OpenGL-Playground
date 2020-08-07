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

    Camera (glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
    {
        Front = glm::vec3(0.0f, 0.0f, -1.0f);
        MovementSpeed = SPEED;
        MouseSensitivity = SENSITIVITY;

        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        updateCameraVectors();
    }

    virtual glm::mat4 getProjectionMatrix() = 0;
    virtual void processMouseScroll(float yoffset) = 0;

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

    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
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

class CameraPerspective : public Camera
{
public:
    float FOV;
    float AspectRatio;

    CameraPerspective(float aspectRatio, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Camera(position, up, yaw, pitch)
    {
        FOV = DEFAULT_FOV;
        AspectRatio = aspectRatio;
    }
    
    glm::mat4 getProjectionMatrix()
    {
        return glm::perspective(glm::radians(FOV), AspectRatio, 0.1f, 100.0f);
    }

    void processMouseScroll(float yoffset)
    {
        FOV -= (float)yoffset;
        if (FOV < 1.0f)
            FOV = 1.0f;
        if (FOV > 90.0f)
            FOV = 90.0f;
    }
};

class CameraOrthographic : public Camera
{
public:
    Vector2f Dimensions;

    CameraOrthographic(Vector2f dimensions, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Camera(position, up, yaw, pitch)
    {
        Dimensions = dimensions;
    }

    glm::mat4 getProjectionMatrix()
    {
        float halfX = Dimensions.x * 0.5f;
        float halfY = Dimensions.y * 0.5f;
        return glm::ortho(-halfX, halfX, -halfY, halfY, 0.1f, 100.0f);
    }

    void processMouseScroll(float yoffset)
    {
        
    }
};

