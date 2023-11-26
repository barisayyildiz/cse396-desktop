//#pragma once
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <vector>
//
//enum Camera_Movement {
//    FORWARD,
//    BACKWARD,
//    LEFT,
//    RIGHT
//};
//
//const float YAW = -90.0f;
//const float PITCH = 0.0f;
//const float SPEED = 2.5f;
//const float SENSITIVITY = 0.1f;
//const float ZOOM = 45.0f;
//
//
//class Camera {
//public:
//    glm::vec3 Position;
//    glm::vec3 Front;
//    glm::vec3 Up;
//    glm::vec3 Right;
//    glm::vec3 WorldUp;
//    float Yaw;
//    float Pitch;
//    float MovementSpeed;
//    float MouseSensitivity;
//    float Zoom;
//
//    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
//        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
//        Position = position;
//        WorldUp = up;
//        Yaw = yaw;
//        Pitch = pitch;
//        updateCameraVectors();
//    }
//
//    glm::mat4 GetViewMatrix() {
//        return glm::lookAt(Position, Position + Front, Up);
//    }
//
//    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
//        float velocity = MovementSpeed * deltaTime;
//
//        auto y = Position.y;
//
//        if (direction == FORWARD)
//            Position += Front * velocity;
//        if (direction == BACKWARD)
//            Position -= Front * velocity;
//        if (direction == LEFT)
//            Position -= Right * velocity;
//        if (direction == RIGHT)
//            Position += Right * velocity;
//
//        //Position.y = y;
//    }
//
//    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
//        xoffset *= MouseSensitivity;
//        yoffset *= MouseSensitivity;
//
//        Yaw += xoffset;
//        Pitch += yoffset;
//
//        if (constrainPitch) {
//            if (Pitch > 89.0f)
//                Pitch = 89.0f;
//            if (Pitch < -89.0f)
//                Pitch = -89.0f;
//        }
//
//        updateCameraVectors();
//    }
//
//    void ProcessMouseScroll(float yoffset) {
//        Zoom -= (float)yoffset;
//        if (Zoom < 1.0f)
//            Zoom = 1.0f;
//        if (Zoom > 45.0f)
//            Zoom = 45.0f;
//    }
//
//private:
//    void updateCameraVectors()
//    {
//        glm::vec3 front;
//        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        front.y = sin(glm::radians(Pitch));
//        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        Front = glm::normalize(front);
//        Right = glm::normalize(glm::cross(Front, WorldUp));
//        Up = glm::normalize(glm::cross(Right, Front));
//    }
//};


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

 //Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float ZOOM = 5.0f;
const float DISTANCE = 0.0001f; // Initial distance from the target

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

            qDebug() << "Yaw: " << Yaw;
            qDebug() << "Pitch: " << Pitch;

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
        if (Zoom > 45.0f)
            Zoom = 45.0f;
        updateCameraPosition();
    }

private:
    void updateCameraVectors()
    {
        //Calculate the new front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);

        //Re-calculate the right and up vectors
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));

        //Update the camera's position based on the new orientation
        updateCameraPosition();
    }

    void updateCameraPosition()
    {
        Position = Target - Front * Zoom;
    }
};
