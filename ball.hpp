#pragma once
#include <glm/glm.hpp>

void loadBallShaders();

class Ball {
public:
    glm::vec3 Position;
    glm::vec3 Velocity;
    float Radius;

    Ball(glm::vec3 position, float radius);

    void update(float deltaTime);
    void draw();
};
