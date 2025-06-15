#ifndef ORBIT_H
#define ORBIT_H

#include <glad/glad.h>
#include <vector>

class Orbit {
private:
    GLuint VAO = 0, VBO = 0;
    std::vector<float> vertices; 
    float radius;

public:
    // Creates orbit path as a circle in XZ plane
    Orbit(float radius, int segments = 100);

    // Releases OpenGL buffers
    ~Orbit();

    // Renders the orbit using GL_LINE_LOOP
    void Draw() const;

    // Getter for orbit radius
    float getRadius() const;
};

#endif
