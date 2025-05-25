#version 330 core

in vec3 WorldPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;

uniform vec3 lightColor;
uniform vec3 lightPos;

uniform vec3 lightColor2;
uniform vec3 lightPos2;

uniform vec3 viewPos;

vec4 getTerrainColor(float height) {
    if (height < -1.0)
        return texture(sandTexture, WorldPos.xz * 0.1);
    else if (height < 1.0)
        return texture(grassTexture, WorldPos.xz * 0.1);
    else if (height < 3.0)
        return texture(rockTexture, WorldPos.xz * 0.1);
    else
        return texture(snowTexture, WorldPos.xz * 0.1);
}

void main() {
    // --- UZUPEŁNIONE BRAKI ---
    vec3 norm = normalize(Normal);
    vec3 baseColor = getTerrainColor(WorldPos.y).rgb;

    // --- ŚWIATŁO 1 ---
    vec3 lightDir1 = normalize(lightPos - WorldPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    float dist1 = length(lightPos - WorldPos);
    float att1 = 1.0 / (1.0 + 0.09 * dist1 + 0.032 * dist1 * dist1);
    vec3 diffuse1 = diff1 * lightColor * baseColor * att1 * 12;

    // --- ŚWIATŁO 2 ---
    vec3 lightDir2 = normalize(lightPos2 - WorldPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    float dist2 = length(lightPos2 - WorldPos);
    float att2 = 1.0 / (1.0 + 0.09 * dist2 + 0.032 * dist2 * dist2);
    vec3 diffuse2 = diff2 * lightColor2 * baseColor * att2 *12;

    // --- Ambient wspólny ---
    vec3 ambient = 0.2 * (lightColor + lightColor2) * baseColor;

    vec3 result = ambient + diffuse1 + diffuse2;
    FragColor = vec4(result, 1.0);
}
