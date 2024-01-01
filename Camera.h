#ifndef H_CAMERA
#define H_CAMERA

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ZOOM = 5.0f;
const float DISTANCE = 0.0001f;

class OrbitCamera
{
public:
    //Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    //Euler Angles
    float Yaw;
    float Pitch;
    //Camera options
    float Zoom;

    float OrbitSpeed = 0.1f;
    float ZoomSpeed = 1.0f;
    float PanSpeed = 0.001f;

    //Target position
    glm::vec3 Target;

    //Constructor with vectors
    OrbitCamera(glm::vec3 target = glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), Zoom(ZOOM)
    {
        Target = target;
        Yaw = yaw;
        Pitch = pitch;
        WorldUp = up;
        updateCameraVectors();
    }

    //Get the view matrix for the orbit camera
    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, Target, Up);
    }

    //Process input to update the camera's position
    void ProcessInput(float xoffset, float yoffset, bool pan = false)
    {
        if (pan)
        {
            Target += Right * xoffset * PanSpeed;
            Target += Up * yoffset * PanSpeed;

            updateCameraPosition();
        }
        else
        {
            Yaw += xoffset * OrbitSpeed;
            Pitch += yoffset * OrbitSpeed;

            //Clamp the pitch to avoid flipping
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;

            updateCameraVectors();
        }
    }

    //Process zoom input
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= yoffset * ZoomSpeed;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 225.0f)
            Zoom = 225.0f;
        updateCameraPosition();
    }

private:
    void updateCameraVectors()
    {
        // Calculate the new front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        // Ensure that the pitch stays within reasonable bounds
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        Front = glm::normalize(front);

        // Re-calculate the right and up vectors
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));

        // Update the camera's position based on the new orientation
        updateCameraPosition();
    }

    void updateCameraPosition()
    {
        Position = Target - Front * Zoom;
    }
};

#endif
