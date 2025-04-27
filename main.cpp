#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "terrain.hpp"
#include "camera.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "textures.hpp"
#include "ball.hpp"

// <--- TU DODAJ TO!!! --->
glm::mat4 globalViewMatrix;
glm::mat4 globalProjectionMatrix;

float aspectRatio = 1.0f;
GLuint shaderProgram;

Camera camera(glm::vec3(0.0f, 10.0f, 20.0f));
bool keys[1024];
std::vector<Ball> balls; 


std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Nie mo¿na otworzyæ pliku: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

// NOWA FUNKCJA:
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
}

void processMovement(float deltaTime) {
    for (int i = 0; i < 1024; i++) {
        if (keys[i]) {
            camera.processKeyboard(i, deltaTime);
        }
    }

    if (keys[GLFW_KEY_ENTER]) {
        glm::vec3 ballPos = camera.Position + camera.Front * 2.0f; // 2 jednostki przed kamer¹
        balls.push_back(Ball(ballPos, 0.5f));
        keys[GLFW_KEY_ENTER] = false; // ¿eby dodawaæ tylko raz na klik
    }
}

void drawScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    glm::mat4 view = camera.getViewMatrix();

    // <<<<< TU DODAJ >>>>>
    globalViewMatrix = view;
    globalProjectionMatrix = projection;
    // <<<<< TU DODAJ >>>>>

    glUseProgram(shaderProgram);

    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sandTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "sandTexture"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "grassTexture"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, rockTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "rockTexture"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, snowTexture);
    glUniform1i(glGetUniformLocation(shaderProgram, "snowTexture"), 3);

    drawTerrain();

    // --- rysowanie wszystkich kulek ---
    for (auto& ball : balls) {
        ball.draw();
    }
    // ------------------------------------

    glfwSwapBuffers(window);
}





int main() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cerr << "Nie mo¿na zainicjowaæ GLFW!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Teren 3D", NULL, NULL);
    if (!window) {
        std::cerr << "Nie mo¿na utworzyæ okna!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Nie mo¿na za³adowaæ GLAD!\n";
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);

    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    shaderProgram = loadShaders("vertex_shader.glsl", "fragment_shader.glsl");
    loadBallShaders();
    loadTextures();

    srand(time(NULL));
    generateTerrain(100, 0.5f);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float lastFrameTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        processMovement(deltaTime);
        drawScene(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
