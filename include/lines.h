#pragma once

#include <glm.hpp>
#include <vector>
#include <glad/glad.h>

#include "shader.h"

struct LineVertex
{
    glm::vec3 pos;
    glm::vec3 color;

    LineVertex(glm::vec3 _pos, glm::vec3 _color)
    {
        pos = _pos;
        color = _color;
    }
};

extern std::vector<LineVertex> lines;

void draw_lines();

void line(glm::vec3 start, glm::vec3 end, glm::vec3 color);
void box(glm::vec3 start, glm::vec3 end, glm::vec3 color);
void triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color);
void tetrahedron(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec3 color);
void point(glm::vec3 pos, glm::vec3 color, float size = 0.02);
void grid(glm::vec3 pos, glm::vec3 normal, glm::vec3 right, float spacing, int size, glm::vec3 color);
void trajectory(const std::vector<glm::vec3>& points, glm::vec3 color);
void bezier(glm::vec3 start, glm::vec3 end, glm::vec3 control, glm::vec3 color);