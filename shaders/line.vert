#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 _color;

uniform mat4 cam_mat;

out vec3 color;

void main()
{
    gl_Position = cam_mat * vec4(pos, 1.0f);
    color = _color;
}