#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;
uniform vec3 objectColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir);

    float diff = max(dot(norm, light), 0.0);

    float ambient = 0.2;

    vec3 color = (ambient + diff) * objectColor;

    FragColor = vec4(color, 1.0);
}