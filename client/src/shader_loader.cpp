#include "shader_loader.hpp"
#include "gl_includes.hpp"
#include <vector>
#include <spdlog/spdlog.h>

namespace Enflopio
{
    static void error_callback(int error, const char *description)
    {
        spdlog::error("Error: {}\n", description);
    }

    void check_error(GLuint shader)
    {
        GLint result;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            GLint log_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
            std::vector<GLchar> log(log_length);

            GLsizei length;
            glGetShaderInfoLog(shader, log.size(), &length, log.data());

            error_callback(0, log.data());
        }
    }

    Shader ShaderLoader::FromText(std::string_view vertex_text,
                                  std::string_view fragment_text)
    {
        auto* vertex_data = vertex_text.data();
        auto* fragment_data = fragment_text.data();
        auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_data, NULL);
        glCompileShader(vertex_shader);
        check_error(vertex_shader);

        auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_data, NULL);
        glCompileShader(fragment_shader);
        check_error(fragment_shader);

        auto program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        Shader result(program);

        return result;
    }
}
