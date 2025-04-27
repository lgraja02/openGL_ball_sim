// camera.hpp
#pragma once
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;

    float Speed;

    Camera(glm::vec3 position);

    void processKeyboard(int key, float deltaTime);
    glm::mat4 getViewMatrix();
};
