#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 initPosition, glm::vec3 initTarget, glm::vec3 initUp) {
        this->position = initPosition;
        this->target = initTarget;
        this->front = glm::normalize(initTarget - initPosition);
        this->up = initUp;
        this->right = glm::normalize(glm::cross(this->front, this->up));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(position, position + front, up);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION dir, float velocity) {
        //TODO
        switch (dir)
        {
        case MOVE_FORWARD:
            if (position.y < 2.0)
            {
                position.y = 2.0;
            }
            else
            {
                position += front * velocity;
            }
            break;

        case MOVE_BACKWARD:
            position -= front * velocity * 0.8f; // Apply a slower backward speed
            break;

        case MOVE_RIGHT:
            position += right * (velocity + 0.1f); // Slightly faster right movement
            break;

        case MOVE_LEFT:
            position -= right * (velocity - 0.05f); // Slightly slower left movement
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float verticalAngle, float horizontalAngle) {
        //TODO

        glm::vec3 newFront;
        newFront.x = cos(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));
        newFront.y = sin(glm::radians(verticalAngle)) + 0.1f; // Add a slight offset to simulate tilt
        newFront.z = sin(glm::radians(horizontalAngle)) * cos(glm::radians(verticalAngle));

        front = glm::normalize(newFront);
    }
}
