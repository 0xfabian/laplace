#include "mesh.h"

using namespace glm;

Mesh::Mesh(const std::vector<Vertex>& _vertices, const std::vector<int>& _indices)
{
    vertices = _vertices;
    indices = _indices;

    build();
}

Mesh::Mesh(const char* path)
{
    load(path);
}

void Mesh::load(const char* path)
{
    char* data = read_from_file(path);

    if (!data)
    {
        printf("Failed to load mesh data\n");
        return;
    }

    int i = 0;
    std::string line;

    std::vector<vec3> normals;

    while (data[i])
    {
        if (data[i] == '\n')
        {
            if (line[0] == 'v' && line[1] == ' ')
            {
                float x, y, z;

                sscanf(line.c_str(), "v %f %f %f", &x, &y, &z);

                vertices.push_back(Vertex(vec3(x, y, z), vec3(0, 0, 0)));
            }
            else if (line[0] == 'v' && line[1] == 'n')
            {
                float x, y, z;

                sscanf(line.c_str(), "vn %f %f %f", &x, &y, &z);

                normals.push_back(vec3(x, y, z));
            }
            else if (line[0] == 'f')
            {
                int v1, v2, v3;
                int n1, n2, n3;

                sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);

                vertices[v1 - 1].normal = normals[n1 - 1];
                vertices[v2 - 1].normal = normals[n2 - 1];
                vertices[v3 - 1].normal = normals[n3 - 1];

                indices.push_back(v1 - 1);
                indices.push_back(v3 - 1);
                indices.push_back(v2 - 1);
            }

            line.clear();
        }
        else
            line.push_back(data[i]);

        i++;
    }

    free(data);

    build();
}

void Mesh::draw()
{
    glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::build()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STREAM_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
}

void Mesh::recalculate_normals()
{
    for (auto& v : vertices)
        v.normal = vec3(0);

    for (int i = 0; i < indices.size(); i += 3)
    {
        int a = indices[i + 0];
        int b = indices[i + 1];
        int c = indices[i + 2];

        vec3 ab = vertices[b].pos - vertices[a].pos;
        vec3 ac = vertices[c].pos - vertices[a].pos;

        vec3 normal = normalize(cross(ac, ab));

        vertices[a].normal += normal;
        vertices[b].normal += normal;
        vertices[c].normal += normal;
    }

    for (auto& v : vertices)
        v.normal = normalize(v.normal);
}

float ray_vs_triangle(const vec3& origin, const vec3& dir, const vec3& a, const vec3& b, const vec3& c)
{
    const float epsilon = 1e-6;

    vec3 edge1 = b - a;
    vec3 edge2 = c - a;
    vec3 ray_cross_e2 = cross(dir, edge2);
    float det = dot(edge1, ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return INFINITY;

    float inv_det = 1.0 / det;
    vec3 s = origin - a;
    float u = inv_det * dot(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return INFINITY;

    vec3 s_cross_e1 = cross(s, edge1);
    float v = inv_det * dot(dir, s_cross_e1);

    if (v < 0 || u + v > 1)
        return INFINITY;

    float t = inv_det * dot(edge2, s_cross_e1);

    if (t > epsilon)
        return t;

    return INFINITY;
}

int Mesh::intersect(const vec3& origin, const vec3& dir)
{
    int index = -1;
    float t_min = INFINITY;

    for (int i = 0; i < indices.size(); i += 3)
    {
        vec3 a = vertices[indices[i]].pos;
        vec3 b = vertices[indices[i + 1]].pos;
        vec3 c = vertices[indices[i + 2]].pos;

        float t = ray_vs_triangle(origin, dir, a, b, c);

        if (t < t_min)
        {
            t_min = t;
            index = i;
        }
    }

    return index;
}

void Mesh::rebuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
}

void Mesh::destroy()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
}