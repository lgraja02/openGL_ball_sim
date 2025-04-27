#version 330 core
in vec3 fragPos;
out vec4 FragColor;

void main()
{
    float height = fragPos.y;
    vec3 color = vec3(0.2, 0.5 + height, 0.2); 
    FragColor = vec4(color, 1.0);
}
