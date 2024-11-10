#include <lines.h>

using namespace glm;

std::vector<LineVertex> lines;

void draw_lines()
{
    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, lines.size() * sizeof(LineVertex), lines.data(), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, lines.size());

    lines.clear();
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void line(vec3 start, vec3 end, vec3 color)
{
    lines.push_back(LineVertex(start, color));
    lines.push_back(LineVertex(end, color));
}

void box(vec3 start, vec3 end, vec3 color)
{
    line(vec3(start.x, start.y, start.z), vec3(end.x, start.y, start.z), color);
    line(vec3(end.x, start.y, start.z), vec3(end.x, start.y, end.z), color);
    line(vec3(end.x, start.y, end.z), vec3(start.x, start.y, end.z), color);
    line(vec3(start.x, start.y, end.z), vec3(start.x, start.y, start.z), color);

    line(vec3(start.x, end.y, start.z), vec3(end.x, end.y, start.z), color);
    line(vec3(end.x, end.y, start.z), vec3(end.x, end.y, end.z), color);
    line(vec3(end.x, end.y, end.z), vec3(start.x, end.y, end.z), color);
    line(vec3(start.x, end.y, end.z), vec3(start.x, end.y, start.z), color);

    line(vec3(start.x, start.y, start.z), vec3(start.x, end.y, start.z), color);
    line(vec3(end.x, start.y, start.z), vec3(end.x, end.y, start.z), color);
    line(vec3(start.x, start.y, end.z), vec3(start.x, end.y, end.z), color);
    line(vec3(end.x, start.y, end.z), vec3(end.x, end.y, end.z), color);
}

void triangle(vec3 a, vec3 b, vec3 c, vec3 color)
{
    line(a, b, color);
    line(b, c, color);
    line(c, a, color);
}

void tetrahedron(vec3 a, vec3 b, vec3 c, vec3 d, vec3 color)
{
    line(a, b, color);
    line(a, c, color);
    line(a, d, color);
    line(b, c, color);
    line(b, d, color);
    line(c, d, color);
}

void point(vec3 pos, vec3 color, float size)
{
    line(vec3(-size, 0, 0) + pos, vec3(size, 0, 0) + pos, color);
    line(vec3(0, -size, 0) + pos, vec3(0, size, 0) + pos, color);
    line(vec3(0, 0, -size) + pos, vec3(0, 0, size) + pos, color);
}

void grid(vec3 pos, vec3 normal, vec3 right, float spacing, int size, vec3 color)
{
    vec3 perp = cross(normal, right);

    for (int i = -size; i <= size; i++)
    {
        vec3 off = pos + (float)i * spacing * perp;
        line(-right * spacing * (float)size + off, right * spacing * (float)size + off, i ? color : vec3(0.5, 0, 0));
    }

    for (int i = -size; i <= size; i++)
    {
        vec3 off = pos + (float)i * spacing * right;
        line(-perp * spacing * (float)size + off, perp * spacing * (float)size + off, i ? color : vec3(0, 0, 0.5));
    }
}

void trajectory(const std::vector<glm::vec3>& points, glm::vec3 color)
{
    for (int i = 0; i < points.size() - 1; i++)
        line(points[i], points[i + 1], color);
}

void bezier(glm::vec3 start, glm::vec3 end, glm::vec3 control, glm::vec3 color)
{
    int n = 7;
    std::vector<glm::vec3> points;

    for (int i = 0; i < n; i++)
    {
        float t = i / (float)(n - 1);
        glm::vec3 point = (1 - t) * (1 - t) * start + 2 * (1 - t) * t * control + t * t * end;

        points.push_back(point);
    }

    trajectory(points, color);
}
