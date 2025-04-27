//ball.hpp
#pragma once
#include <glm/glm.hpp>

void loadBallShaders();

class Ball {
public:
    glm::vec3 Position;
    float Radius;

    Ball(glm::vec3 position, float radius);

    void draw();
};
