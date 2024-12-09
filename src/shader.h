#ifndef SHADER_H
#define SHADER_H

const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos; // Vertex Position
layout (location = 1) in vec3 aColor; // Vertex Color

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

in vec3 ourColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

#endif