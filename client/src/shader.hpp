#pragma once
#include "GlIncludes.hpp"
#include <glm/glm.hpp>
#include <string_view>
#include <glm/gtc/type_ptr.hpp>

namespace Enflopio
{
    class ShaderLoader;

    class Shader
    {
        friend class ShaderLoader;
    public:
        void Use() { glUseProgram(m_program); }

        void SetUniform(const char* name, float value)
        {
            glUniform1f(glGetUniformLocation(m_program, name), value);
        }
        void SetUniform(const char* name, int value);
        void SetUniform(const char* name, unsigned value);
        void SetUniform(const char* name, const glm::mat4& mat)
        {
            glUniformMatrix4fv(glGetUniformLocation(m_program, name),
                               1, GL_FALSE, glm::value_ptr(mat));
        }
        void SetUniform(const char* name, const glm::vec2& vec)
        {
            glUniform2fv(glGetUniformLocation(m_program, name),
                         1, glm::value_ptr(vec));
        }
        void SetUniform(const char* name, const glm::vec3& mat);
        void SetUniform(const char* name, const glm::vec4& vec)
        {
            glUniform4fv(glGetUniformLocation(m_program, name),
                         1, glm::value_ptr(vec));
        }

        ~Shader()
        {
            glDeleteProgram(m_program);
        }
    private:
        Shader(GLuint program)
            : m_program(program)
        {
        }

        GLuint m_program;
    };
}
