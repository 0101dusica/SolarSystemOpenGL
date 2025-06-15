#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

// Stores glyph texture and its metrics
struct Character {
    unsigned int TextureID;  // ID of glyph texture
    glm::ivec2 Size;         // Size of glyph
    glm::ivec2 Bearing;      // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Horizontal offset to advance to next glyph
};

class Text {
public:
    std::map<char, Character> Characters; // Loaded characters
    unsigned int VAO, VBO;                // Buffers for rendering
    Shader shader;                        // Shader used for text rendering

    // Initializes text rendering with given font
    Text(const std::string& fontPath, int fontSize);

    // Draws text string on screen
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color);

    // Sets the projection matrix for the text shader
    void SetProjection(const glm::mat4& projection);
};

#endif