#include "Camera.h"
#include <glm/gtc/constants.hpp>
#include <algorithm> // for std::clamp

// Constructor: initialize spherical coordinates and calculate position
Camera::Camera(float radius, float theta, float phi)
    : Radius(radius), Theta(theta), Phi(phi)
{
    updatePosition();
}

// Recalculate camera position and orientation based on spherical coordinates
void Camera::updatePosition()
{
    // Convert spherical to Cartesian coordinates
    Position.x = Radius * cos(Theta) * sin(Phi);
    Position.y = Radius * sin(Theta);
    Position.z = Radius * cos(Theta) * cos(Phi);

    // Camera looks toward origin (0,0,0)
    Front = glm::normalize(-Position);

    // Compute camera's Right and Up vectors using cross products
    Right = glm::normalize(glm::cross(Front, glm::vec3(0.0f, 1.0f, 0.0f)));
    Up = glm::normalize(glm::cross(Right, Front));
}

// Generate view matrix using glm::lookAt
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, glm::vec3(0.0f), Up);
}

// Modify Phi/Theta based on direction and time, then update camera position
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    const float angleSpeed = 1.0f * deltaTime;

    if (direction == LEFT)
        Phi -= angleSpeed;
    if (direction == RIGHT)
        Phi += angleSpeed;
    if (direction == UPWARD)
        Theta += angleSpeed;
    if (direction == DOWNWARD)
        Theta -= angleSpeed;

    // Clamp Theta to avoid flipping camera at the poles
    float epsilon = 0.01f;
    Theta = glm::clamp(Theta, -glm::half_pi<float>() + epsilon, glm::half_pi<float>() - epsilon);

    updatePosition();
}
