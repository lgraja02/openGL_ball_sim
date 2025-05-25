#include "ball.hpp"
#include "terrain.hpp" // <- bardzo wa¿ne! (¿eby mieæ heightFunction!)
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <glm/gtc/type_ptr.hpp> 

// Funkcja do ³adowania shaderów
extern GLuint loadShaders(const std::string& vertexPath, const std::string& fragmentPath);

// Zmienne globalne na shader i view/projection
GLuint ballShaderProgram = 0;
extern glm::mat4 globalViewMatrix;
extern glm::mat4 globalProjectionMatrix;


// VAO, VBO, EBO kulki
GLuint ballVAO = 0;
GLuint ballVBO = 0;
GLuint ballEBO = 0;
std::vector<float> ballVertices;
std::vector<unsigned int> ballIndices;

void loadBallShaders() {
    ballShaderProgram = loadShaders("vertex_ball.glsl", "fragment_ball.glsl");
}

void createBallMesh(float radius, int sectors = 18, int stacks = 18) {
    ballVertices.clear();
    ballIndices.clear();

    for (int i = 0; i <= stacks; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stacks;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectors;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            ballVertices.push_back(x);
            ballVertices.push_back(y);
            ballVertices.push_back(z);
            ballVertices.push_back(normal.x);
            ballVertices.push_back(normal.y);
            ballVertices.push_back(normal.z);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                ballIndices.push_back(k1);
                ballIndices.push_back(k2);
                ballIndices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                ballIndices.push_back(k1 + 1);
                ballIndices.push_back(k2);
                ballIndices.push_back(k2 + 1);
            }
        }
    }

    glGenVertexArrays(1, &ballVAO);
    glGenBuffers(1, &ballVBO);
    glGenBuffers(1, &ballEBO);

    glBindVertexArray(ballVAO);

    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    glBufferData(GL_ARRAY_BUFFER, ballVertices.size() * sizeof(float), ballVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ballEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ballIndices.size() * sizeof(unsigned int), ballIndices.data(), GL_STATIC_DRAW);

    // Pozycja
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normalna
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Ball::Ball(glm::vec3 position, float radius)
    : Position(position), Radius(radius), Velocity(0.0f, 0.0f, 0.0f) // <- inicjalizacja Velocity!
{
    if (ballVAO == 0) {
        createBallMesh(radius);
    }
}

void Ball::update(float deltaTime) {
    // --- 1. Grawitacja ---
    Velocity.y -= 9.8f * deltaTime;

    // --- 2. Aktualizacja pozycji ---
    Position += Velocity * deltaTime;

    // --- 3. Kolizja z terenem ---
    float terrainHeight = heightFunction(Position.x, Position.z, 100, 0.5f);

    if (Position.y - Radius < terrainHeight) {
        Position.y = terrainHeight + Radius;

        if (Velocity.y < 0.0f) {
            Velocity.y = 0.0f;
        }

        // --- 4. Staczanie siê po zboczu ---
        float hL = heightFunction(Position.x - 0.5f, Position.z, 100, 0.5f);
        float hR = heightFunction(Position.x + 0.5f, Position.z, 100, 0.5f);
        float hD = heightFunction(Position.x, Position.z - 0.5f, 100, 0.5f);
        float hU = heightFunction(Position.x, Position.z + 0.5f, 100, 0.5f);

        glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 1.0f, hD - hU));
        glm::vec3 downSlope = glm::normalize(glm::vec3(normal.x, 0.0f, normal.z)); // UWAGA: bez minusa!

        Velocity.x += downSlope.x * 2.0f * deltaTime;
        Velocity.z += downSlope.z * 2.0f * deltaTime;

        // --- 5. Tarcie ---
        Velocity.x *= 0.98f;
        Velocity.z *= 0.98f;
    }
}







void Ball::draw() {
    glUseProgram(ballShaderProgram);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), Position);
    model = glm::scale(model, glm::vec3(Radius));

    GLuint modelLoc = glGetUniformLocation(ballShaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(ballShaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(ballShaderProgram, "projection");

    GLuint lightPos1Loc = glGetUniformLocation(ballShaderProgram, "lightPos");
    GLuint lightColor1Loc = glGetUniformLocation(ballShaderProgram, "lightColor");
    GLuint lightPos2Loc = glGetUniformLocation(ballShaderProgram, "lightPos2");
    GLuint lightColor2Loc = glGetUniformLocation(ballShaderProgram, "lightColor2");
    GLuint viewPosLoc = glGetUniformLocation(ballShaderProgram, "viewPos");

    glUniform3fv(lightPos1Loc, 1, glm::value_ptr(glm::vec3(22.0f, 3.0f, 15.0f)));
    glUniform3fv(lightColor1Loc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glUniform3fv(lightPos2Loc, 1, glm::value_ptr(glm::vec3(-22.0f, 12.0f, -15.0f)));
    glUniform3fv(lightColor2Loc, 1, glm::value_ptr(glm::vec3(0.0f, 0.7f, 0.2f)));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(Position)); // Mo¿esz te¿ u¿yæ pozycji kamery


    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &globalViewMatrix[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &globalProjectionMatrix[0][0]);

    glBindVertexArray(ballVAO);

    glDrawElements(GL_TRIANGLES, ballIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
