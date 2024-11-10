#include <shader.h>

Shader::Shader(const std::string& name)
{
    std::string vert_path = "shaders/" + name + ".vert";
    std::string frag_path = "shaders/" + name + ".frag";

    char* vert_data = read_from_file(vert_path.c_str());

    crash(!vert_data, "Failed to read vertex shader source", 1);

    GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_id, 1, &vert_data, 0);
    free(vert_data);

    GLint compiled;
    glCompileShader(vert_id);
    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &compiled);

    crash(compiled != GL_TRUE, "Failed to compile vertex shader", 2);

    char* frag_data = read_from_file(frag_path.c_str());

    crash(!frag_data, "Failed to read fragment shader source", 3);

    GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_id, 1, &frag_data, 0);
    free(frag_data);

    glCompileShader(frag_id);
    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &compiled);

    crash(compiled != GL_TRUE, "Failed to compile fragment shader", 4);

    id = glCreateProgram();

    glAttachShader(id, vert_id);
    glAttachShader(id, frag_id);
    glLinkProgram(id);

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
}

void Shader::bind()
{
    glUseProgram(id);
}

void Shader::upload_float(const char* var_name, float value)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform1f(loc, value);
}

void Shader::upload_int(const char* var_name, int value)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform1i(loc, value);
}

void Shader::upload_vec3(const char* var_name, const glm::vec3& vec)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

void Shader::upload_vec4(const char* var_name, const glm::vec4& vec)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::upload_mat4(const char* var_name, const glm::mat4& matrix)
{
    GLuint loc = glGetUniformLocation(id, var_name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::destroy()
{
    glDeleteProgram(id);
}