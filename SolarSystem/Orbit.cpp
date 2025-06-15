#include "Orbit.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

// Generate circular orbit vertices and set up VAO/VBO
Orbit::Orbit(float radius, int segments) : radius(radius) {
    float angleStep = 2.0f * M_PI / segments;

    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = radius * cosf(angle);
        float z = radius * sinf(angle);
        vertices.push_back(x);
        vertices.push_back(0.0f);
        vertices.push_back(z);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// Delete buffers
Orbit::~Orbit() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// Draw the orbit
void Orbit::Draw() const {
    if (VAO == 0) return;
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, vertices.size() / 3);
    glBindVertexArray(0);
}

float Orbit::getRadius() const {
    return radius;
}
