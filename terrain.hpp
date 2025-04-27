#pragma once
#include <vector>
#include <glad/glad.h>

extern std::vector<float> terrainVertices;
extern std::vector<unsigned int> terrainIndices;
extern GLuint terrainVAO, terrainVBO, terrainEBO;

float heightFunction(float x, float z, int size, float step);
void generateTerrain(int size, float step);
void drawTerrain();
