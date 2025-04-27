#version 330 core
in vec3 fragPos;
out vec4 FragColor;

uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

void main()
{
    float height = fragPos.y;
    vec2 texCoords = fragPos.xz * 0.1; // Mo¿esz zmieniaæ skalowanie tekstur

    if (height < -1.0)
        FragColor = texture(sandTexture, texCoords);
    else if (height < 1.0)
        FragColor = texture(grassTexture, texCoords);
    else if (height < 3.0)
        FragColor = texture(rockTexture, texCoords);
    else
        FragColor = texture(snowTexture, texCoords);
}
