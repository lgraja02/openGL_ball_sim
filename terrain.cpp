#include "terrain.hpp"
#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

std::vector<float> terrainVertices;
std::vector<unsigned int> terrainIndices;
GLuint terrainVAO = 0;
GLuint terrainVBO = 0;
GLuint terrainEBO = 0;

// Parametry generacji terenu
float randomFactor = 10.0f; // wysokoœæ pagórków
int smoothness = 15;       // co ile punktów nowy random
std::vector<std::vector<float>> controlPoints;

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float heightFunction(float x, float z, int size, float step) {
    int i = (x / step) + size / 2;
    int j = (z / step) + size / 2;

    int ci = i / smoothness;
    int cj = j / smoothness;

    float localX = (i % smoothness) / (float)smoothness;
    float localZ = (j % smoothness) / (float)smoothness;

    // Interpolacja liniowa w obu kierunkach
    float h00 = controlPoints[ci][cj];
    float h01 = controlPoints[ci][cj + 1];
    float h10 = controlPoints[ci + 1][cj];
    float h11 = controlPoints[ci + 1][cj + 1];

    float hx0 = lerp(h00, h01, localZ);
    float hx1 = lerp(h10, h11, localZ);

    return lerp(hx0, hx1, localX);
}

void generateTerrain(int size, float step) {
    terrainVertices.clear();
    terrainIndices.clear();

    srand(time(NULL)); // zawsze nowy seed

    // Tworzymy bazowe punkty losowe
    int controlSize = size / smoothness + 2; // +2 na bezpieczny zapas
    controlPoints = std::vector<std::vector<float>>(controlSize, std::vector<float>(controlSize));

    for (int i = 0; i < controlSize; i++) {
        for (int j = 0; j < controlSize; j++) {
            controlPoints[i][j] = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * randomFactor;
        }
    }

    // Generujemy siatkê terenu z interpolacj¹
    for (int i = 0; i <= size; i++) {
        for (int j = 0; j <= size; j++) {
            float x = (i - size / 2) * step;
            float z = (j - size / 2) * step;
            float y = heightFunction(x, z, size, step);

            terrainVertices.push_back(x);
            terrainVertices.push_back(y);
            terrainVertices.push_back(z);
        }
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int start = i * (size + 1) + j;

            terrainIndices.push_back(start);
            terrainIndices.push_back(start + size + 1);
            terrainIndices.push_back(start + 1);

            terrainIndices.push_back(start + 1);
            terrainIndices.push_back(start + size + 1);
            terrainIndices.push_back(start + size + 2);
        }
    }

    // Tworzymy VAO, VBO, EBO
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, terrainVertices.size() * sizeof(float), terrainVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndices.size() * sizeof(unsigned int), terrainIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void drawTerrain() {
    glBindVertexArray(terrainVAO);
    glDrawElements(GL_TRIANGLES, terrainIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
