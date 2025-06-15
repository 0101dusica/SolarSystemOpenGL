#ifndef PLANET_H
#define PLANET_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Orbit.h"

class Planet {
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    GLuint VBO = 0, VAO = 0, EBO = 0;

    float radius;
    int sectorCount;
    int stackCount;

    // Generate sphere vertex data
    void generateVertices();

    // Generate sphere indices
    void generateIndices();

    // Set up VAO, VBO, EBO for rendering
    void setupBuffers();

public:
    float rotationSpeed = 0.0f;
    float orbitSpeed = 0.0f;
    GLuint textureID = 0;

    Orbit* orbit = nullptr;

    // Creates planet with optional orbit radius
    Planet(float r = 1.0f, int sectors = 36, int stacks = 18, float orbitRadius = 0.0f);

    // Clean up OpenGL buffers and orbit
    ~Planet();

    float getRadius() const;

    // Render the sphere
    void Draw() const;

    // Render the orbit if it exists
    void DrawOrbit() const;
};

#endif
