#pragma once

#include <glm.hpp>
#include <ext.hpp>

struct Camera
{
    glm::vec3 pos;
    glm::vec3 forward;
    glm::vec3 up;

    float fov = 80;
    float aspect_ratio;

    float near = 0.01;
    float far = 1000.0;

    glm::mat4 matrix(const glm::vec3& offset);
    void set_aspect(int width, int height);
};