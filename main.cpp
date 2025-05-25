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

glm::mat4 globalViewMatrix;                 // gl macierz widoku (kamera)
glm::mat4 globalProjectionMatrix;           // gl macierz projekcji (przeksztalca widok 3D na 2D ekranu)

float aspectRatio = 1.0f;                   // Proporcje okna 
GLuint shaderProgram;                       // ID programu shader�w

Camera camera(glm::vec3(0.0f, 10.0f, 10.0f)); // pozycja kamery w przestzeni
bool keys[1024];                              // tablica przechowuj�ca stan nacisiecia klawiszy
std::vector<Ball> balls;                      // wektor przechowuj�cy wszystkie kulki

// funkcja odczytuj�ca zawarto�� pliku do stringa SHADERY
std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Nie mo�na otworzy� pliku: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

//fFunkcja tworz�ca program shaderww
GLuint loadShaders(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    const char* vShaderCode = vertexCode.c_str(); //opengl oczekuje const char* 
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);     // inicjalizacja obiektu vertex shader
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);        // laduje kod (shader_id, liczba_linii, tablica_do_tekstu, tablica_d�ugo�ci)
    glCompileShader(vertexShader);                              // kompilacja shadera (przeksztalca kodzik na kod zrozumialy przez GPU)

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();         // pusty program dla shaderww
    glAttachShader(program, vertexShader);      // dodaj vertex shader
    glAttachShader(program, fragmentShader);    // dodaj fragment shader
    glLinkProgram(program);                     // laczy oba w program

    glDeleteShader(vertexShader);   //usuwamy oryginaly bo sa juz w programshader 
    glDeleteShader(fragmentShader);

    return program; // zwracamy program na ktorym bedziemy bazowaz potem
}

//obsluga bledow do konsoli
void error_callback(int error, const char* description) {
    fputs(description, stderr); //wypisanie bledu
}


//obsluga klawiatury
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

//obsluga myszki
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    camera.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos));
}

// Obsluga ruchow i dodawanie kulek
void processMovement(float deltaTime) {
    for (int i = 0; i < 1024; i++) { //sprawdzenie wcisnietych klawiszy
        if (keys[i]) {
            camera.processKeyboard(i, deltaTime); //przekazanie klawisza do kamerki
        }
    }

    if (keys[GLFW_KEY_ENTER]) { //rzut kulka
        glm::vec3 ballPos = camera.Position + camera.Front * 2.0f;
        balls.push_back(Ball(ballPos, 0.5f)); //dodaje kulke do listy, PROMIE�!!!
        keys[GLFW_KEY_ENTER] = false; //zapobiega wielokrotnemu dodawaniu
    }
}

//rysowanie sceny
void drawScene(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //czyszczenie poprzedniej klatki

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f); //macierz projekcji [45stopni g�ra d�, skalowanie na roz okna, renderuje dalej niz 0.1, 100stopni prawo lewo]
    glm::mat4 view = camera.getViewMatrix(); //macierz widoku z kamery

    globalViewMatrix = view; //zapis globalnie
    globalProjectionMatrix = projection;

    glUseProgram(shaderProgram); //z jakich shaderow mamy korzystac v i f

    glm::vec3 lightPos1 = glm::vec3(22.0f, 3.0f, 15.0f);
    glm::vec3 lightColor1 = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 lightPos2 = glm::vec3(-22.0f, 12.0f, -15.0f);
    glm::vec3 lightColor2 = glm::vec3(0.0f, 0.7f, 0.2f); // niebieskawe

    glUseProgram(shaderProgram);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos1));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor1));

    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos2"), 1, glm::value_ptr(lightPos2));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor2"), 1, glm::value_ptr(lightColor2));
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(camera.Position));


    GLint projLoc = glGetUniformLocation(shaderProgram, "projection"); //pobieramy z v shadera perspektywe (3D NA 2D)
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");       //lokacje kamery

    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection)); // Przelanie macierzy do shaderow (AKTUALIZACJA)
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    //Ustawienie tekstur (piasek, trawa, ska�a, �nieg)
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

    for (auto& ball : balls) { //aktualizacjaa kulek
        ball.update(1.0f / 60.0f); //aa�o�ony czas klatki (60 FPS)
    }

    for (auto& ball : balls) { // Rysowanie wszystkich kulek
        ball.draw();
    }

    glfwSwapBuffers(window); // Wy�wietl klatk�
}

// Funkcja g��wna programu
int main() {
    glfwSetErrorCallback(error_callback); // Rejestracja callbacka b��d�w

    if (!glfwInit()) { // Inicjalizacja GLFW
        std::cerr << "Nie mo�na zainicjowa� GLFW!\n";
        return -1;
    }

    // Wymagania wersji OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Teren 3D", NULL, NULL); // Tworzenie okna
    if (!window) { // Sprawdzenie poprawno�ci
        std::cerr << "Nie mo�na utworzy� okna!\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // Ustawienie kontekstu OpenGL

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Inicjalizacja GLAD
        std::cerr << "Nie mo�na za�adowa� GLAD!\n";
        glfwTerminate();
        return -1;
    }

    glfwSwapInterval(1); // W��czenie V-Sync
    glEnable(GL_DEPTH_TEST); // W��czenie bufora g��boko�ci

    glfwSetKeyCallback(window, keyCallback); // Callback klawiatury
    glfwSetCursorPosCallback(window, mouseCallback); // Callback myszy
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Ukrycie kursora

    shaderProgram = loadShaders("vertex_shader.glsl", "fragment_shader.glsl"); // Wczytaj shadery
    loadBallShaders(); // Wczytaj shadery kulki
    loadTextures(); // Wczytaj tekstury

    srand(time(NULL)); // Inicjalizacja losowo�ci
    generateTerrain(100, 0.5f); // Wygeneruj teren 100x100, skala 0.5

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Tryb siatki (debugowanie)

    float lastFrameTime = glfwGetTime(); // Czas ostatniej klatki
    while (!glfwWindowShouldClose(window)) { // G��wna p�tla programu
        float currentFrameTime = glfwGetTime(); // Aktualny czas
        float deltaTime = currentFrameTime - lastFrameTime; // Czas od poprzedniej klatki
        lastFrameTime = currentFrameTime; // Aktualizacja czasu

        processMovement(deltaTime); // Obs�uga wej�cia
        drawScene(window); // Rysowanie sceny
        glfwPollEvents(); // Obs�uga zdarze�
    }

    glfwDestroyWindow(window); // Zniszcz okno
    glfwTerminate(); // Zako�cz GLFW
    return 0; // Zako�cz program
}