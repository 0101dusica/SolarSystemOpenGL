#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define FT_FREETYPE_H   
#include "ft2build.h"

#include <algorithm> 
#include <iostream>
#include <cstdlib>
#define NOMINMAX
#include <wtypes.h>

#include "Camera.h"
#include "Shader.h"
#include "Planet.h"
#include "Text.h"

const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1400;

Camera camera(300.0f, 0.0f, glm::radians(90.0f));
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool rotatePlanets = true;
bool spacePressedLastFrame = false;

// Loads a texture from file using stb_image
unsigned int loadTexture(const char* path);

// Callback to adjust viewport when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Handles keyboard input (WASD + SPACE)
void processInput(GLFWwindow* window);

// Sets up a fullscreen quad for rendering the background texture
void setupQuad(unsigned int& quadVAO, unsigned int& quadVBO);

// Initializes the sun and creates all planet objects with texture and movement properties
void setupPlanets(Planet& sun, std::vector<Planet*>& planets);


int main() {
    // Initialize GLFW and create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, [](GLFWwindow*, double, double yoffset) {
        camera.Radius -= (float)yoffset * 10.0f;
        camera.Radius = std::max(50.0f, camera.Radius);
        camera.updatePosition();
        });

    // Load OpenGL functions using GLAD 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Load shaders and bind texture unit 
    Shader planetShader("planet.vs", "planet.fs");
    Shader backgroundShader("background.vs", "background.fs");
    Shader orbitShader("orbit.vs", "orbit.fs");
    planetShader.Use();
    planetShader.setInt("texture1", 0);

    // Create Sun and planets 
    Planet sun(25.0f, 48, 24);
    std::vector<Planet*> planets;
    setupPlanets(sun, planets);

    // Load background (stars) texture and quad 
    unsigned int starsTexture = loadTexture("assets/stars.jpg");
    unsigned int quadVAO, quadVBO;
    setupQuad(quadVAO, quadVBO);

    // Initialize text rendering system 
    Text myText("C:/Windows/Fonts/arial.ttf", 24);

    static float animationTime = 0.0f;
    static float pauseStart = 0.0f;
    static bool wasPaused = false;

    // Main render loop 
    while (!glfwWindowShouldClose(window)) {
        // Frame timing 
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render background 
        glDisable(GL_DEPTH_TEST);
        backgroundShader.Use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, starsTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);

        // Setup camera projection and view 
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // Render planet orbits 
        orbitShader.Use();
        orbitShader.setMat4("projection", projection);
        orbitShader.setMat4("view", view);
        orbitShader.setVec3("orbitColor", glm::vec3(0.6f));
        orbitShader.setMat4("model", glm::mat4(1.0f));
        for (const auto& planet : planets) {
            planet->DrawOrbit();
        }

        // Handle pause/play animation 
        if (rotatePlanets) {
            if (wasPaused) {
                animationTime += glfwGetTime() - pauseStart;
                wasPaused = false;
            }
        }
        else {
            if (!wasPaused) {
                pauseStart = glfwGetTime();
                wasPaused = true;
            }
        }

        float t = rotatePlanets ? glfwGetTime() - animationTime : pauseStart - animationTime;

        // Render Sun 
        planetShader.Use();
        planetShader.setMat4("projection", projection);
        planetShader.setMat4("view", view);

        glm::mat4 sunModel = glm::rotate(glm::mat4(1.0f), t * sun.rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
        planetShader.setMat4("model", sunModel);
        glBindTexture(GL_TEXTURE_2D, sun.textureID);
        sun.Draw();

        // Render planets with rotation and orbit 
        for (auto* planet : planets) {
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), t * planet->orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, glm::vec3(
                planet->orbit ? planet->orbit->getRadius() : 0.0f, 0.0f, 0.0f));
            model = glm::rotate(model, t * planet->rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            planetShader.setMat4("model", model);
            glBindTexture(GL_TEXTURE_2D, planet->textureID);
            planet->Draw();
        }

        // Render text
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 orthoProj = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
        myText.SetProjection(orthoProj);

        float x = width - 300.0f;
        float y = 30.0f;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        myText.RenderText("SV 42/2021 Dusica Trbovic", x, y, 1.0f, glm::vec3(1, 1, 1));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(UPWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWNWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressedLastFrame) {
            rotatePlanets = !rotatePlanets;
            spacePressedLastFrame = true;
        }
    }
    else {
        spacePressedLastFrame = false;
    }
}

void setupQuad(unsigned int& quadVAO, unsigned int& quadVBO) {
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void setupPlanets(Planet& sun, std::vector<Planet*>& planets) {
    sun.rotationSpeed = 0.2f;
    sun.textureID = loadTexture("assets/sun.jpg");

    Planet* mercury = new Planet(2.0f, 36, 18, 40.0f);
    mercury->rotationSpeed = 0.02f;
    mercury->orbitSpeed = 4.17f;
    mercury->textureID = loadTexture("assets/mercury.jpg");

    Planet* venus = new Planet(3.0f, 36, 18, 60.0f);
    venus->rotationSpeed = -0.00f;
    venus->orbitSpeed = 1.61f;
    venus->textureID = loadTexture("assets/venus.jpg");

    Planet* earth = new Planet(3.0f, 36, 18, 85.0f);
    earth->rotationSpeed = 1.0f;
    earth->orbitSpeed = 1.0f;
    earth->textureID = loadTexture("assets/earth.jpg");

    Planet* mars = new Planet(2.5f, 36, 18, 110.0f);
    mars->rotationSpeed = 0.97f;
    mars->orbitSpeed = 0.53f;
    mars->textureID = loadTexture("assets/mars.jpg");

    Planet* jupiter = new Planet(7.0f, 36, 18, 150.0f);
    jupiter->rotationSpeed = 2.4f;
    jupiter->orbitSpeed = 0.084f;
    jupiter->textureID = loadTexture("assets/jupiter.jpg");

    Planet* saturn = new Planet(6.0f, 36, 18, 230.0f);
    saturn->rotationSpeed = 2.27f;
    saturn->orbitSpeed = 0.034f;
    saturn->textureID = loadTexture("assets/saturn.jpg");

    Planet* uranus = new Planet(4.0f, 36, 18, 300.0f);
    uranus->rotationSpeed = -1.39f;
    uranus->orbitSpeed = 0.012f;
    uranus->textureID = loadTexture("assets/uranus.jpg");

    planets = { mercury, venus, earth, mars, jupiter, saturn, uranus };
}