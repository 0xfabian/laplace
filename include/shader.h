#pragma once

#include <string>
#include <glad/glad.h>
#include <ext.hpp>

#include <misc.h>

class Shader
{
    GLuint id;

public:

    Shader() {}
    Shader(const std::string& name);

    void bind();

    void upload_float(const char* var_name, float value);
    void upload_int(const char* var_name, int value);
    void upload_vec3(const char* var_name, const glm::vec3& vec);
    void upload_vec4(const char* var_name, const glm::vec4& vec);
    void upload_mat4(const char* var_name, const glm::mat4& matrix);

    void destroy();
};