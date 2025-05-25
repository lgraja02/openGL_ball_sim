#pragma once
#include <string>
#include <glad/glad.h>

void loadTextures();
GLuint getTextureForHeight(float height);

extern GLuint sandTexture;
extern GLuint grassTexture;
extern GLuint rockTexture;
extern GLuint snowTexture;
