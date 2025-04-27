#include <iostream>
#include "textures.hpp"
#include "stb_image.h"  // TYLKO include, BEZ define!

GLuint sandTexture;
GLuint rockTexture;
GLuint grassTexture;
GLuint snowTexture;

GLuint loadTextureFromFile(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format = (nrComponents == 3) ? GL_RGB : GL_RGBA;

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
        std::cerr << "B³¹d: Nie uda³o siê za³adowaæ pliku tekstury: " << path << std::endl;
    }

    return textureID;
}

void loadTextures() {
    sandTexture = loadTextureFromFile("assets/sand.png");
    rockTexture = loadTextureFromFile("assets/rock.png");
    grassTexture = loadTextureFromFile("assets/grass.png");
    snowTexture = loadTextureFromFile("assets/snow.png");
}

GLuint getTextureForHeight(float height) {
    if (height < -1.0f)
        return sandTexture;
    else if (height < 1.0f)
        return grassTexture;
    else if (height < 3.0f)
        return rockTexture;
    else
        return snowTexture;
}
