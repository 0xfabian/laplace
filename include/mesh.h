#pragma once

#include <vector>
#include <glm.hpp>
#include <glad/glad.h>
#include <string>

#include "misc.h"

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;

    Vertex(const glm::vec3& _pos, const glm::vec3& _norm, const glm::vec3& _color = glm::vec3(0))
    {
        pos = _pos;
        normal = _norm;
        color = _color;
    }
};

class Mesh
{
    GLuint vao;
    GLuint vbo;
    GLuint ebo;

public:

    std::vector<Vertex> vertices;
    std::vector<int> indices;

    Mesh() {}
    Mesh(const std::vector<Vertex>& _vertices, const std::vector<int>& _indices);
    Mesh(const char* path);

    void load(const char* path);
    void draw();
    void build();
    void recalculate_normals();
    int intersect(const glm::vec3& origin, const glm::vec3& dir);
    void rebuffer();
    void destroy();
};