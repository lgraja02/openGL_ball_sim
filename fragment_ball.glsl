#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 lightPos2;
uniform vec3 lightColor2;
uniform vec3 viewPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Œwiat³o 1
    vec3 lightDir1 = normalize(lightPos - FragPos);
    float diff1 = max(dot(norm, lightDir1), 0.0);
    float dist1 = length(lightPos - FragPos);
    float att1 = 1.0 / (1.0 + 0.09 * dist1 + 0.032 * dist1 * dist1);
    vec3 diffuse1 = diff1 * lightColor * att1;

    // Œwiat³o 2
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    float dist2 = length(lightPos2 - FragPos);
    float att2 = 1.0 / (1.0 + 0.09 * dist2 + 0.032 * dist2 * dist2);
    vec3 diffuse2 = diff2 * lightColor2 * att2;

    vec3 ambient = vec3(0.2);

    vec3 result = (ambient + diffuse1 + diffuse2);
    FragColor = vec4(result, 1.0);
}
