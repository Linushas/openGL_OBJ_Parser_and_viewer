#ifndef SHADER_H
#define SHADER_H

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos; // Vertex Position
layout (location = 1) in vec3 aColor; // Vertex Color
layout (location = 2) in vec3 aNormal; // Vertex Normal

flat out vec3 Normal;
out vec3 FragPos;
flat out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = aNormal;
    ourColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

flat in vec3 Normal;
in vec3 FragPos;
flat in vec3 ourColor;

out vec4 FragColor;

uniform vec3 lightPos; // Position of the light source
uniform vec3 viewPos; // Position of the viewer
uniform vec3 lightColor; // Color of the light source
uniform vec3 objectColor; // Color of the object

void main() {
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine the lighting components
    vec3 result = (ambient + diffuse + specular) * ourColor;
    FragColor = vec4(result, 1.0);
}
)";

#endif