#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

out vec3 norm;
out vec3 col;

uniform mat4 cam_mat;

void main()
{
    gl_Position = cam_mat * vec4(pos, 1.0f);

    norm = normal;
    col = color;
}