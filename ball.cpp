#include "ball.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>

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

// Funkcja ³adowania shaderów dla kulki
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

            ballVertices.push_back(x);
            ballVertices.push_back(y);
            ballVertices.push_back(z);
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizei)ballVertices.size() * sizeof(float), ballVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ballEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)ballIndices.size() * sizeof(unsigned int), ballIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Ball::Ball(glm::vec3 position, float radius)
    : Position(position), Radius(radius)
{
    if (ballVAO == 0) {
        createBallMesh(radius);
    }
}

void Ball::draw() {
    glUseProgram(ballShaderProgram);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), Position);
    model = glm::scale(model, glm::vec3(Radius));

    GLuint modelLoc = glGetUniformLocation(ballShaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(ballShaderProgram, "view");
    GLuint projLoc = glGetUniformLocation(ballShaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &globalViewMatrix[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &globalProjectionMatrix[0][0]);

    glBindVertexArray(ballVAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)ballIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
