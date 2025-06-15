#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Enum for controlling camera movement via keyboard
enum Camera_Movement {
    LEFT,
    RIGHT,
    UPWARD,
    DOWNWARD
};

class Camera {
public:
    glm::vec3 Position;  // Current camera position in world space
    glm::vec3 Front;     // Direction from camera to target (always points to origin)
    glm::vec3 Up;        // Camera's up vector
    glm::vec3 Right;     // Perpendicular to Up and Front

    float Radius; // Distance from the origin (used for orbiting)
    float Theta;  // Vertical angle (up/down)
    float Phi;    // Horizontal angle (left/right)

    // Constructor with optional spherical coordinate parameters
    Camera(float radius = 300.0f, float theta = 0.0f, float phi = glm::radians(90.0f));

    // Update angles based on input direction
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // Return view matrix using current position
    glm::mat4 GetViewMatrix() const;

    // Recalculate position and direction vectors from spherical coordinates
    void updatePosition();
};

#endif
