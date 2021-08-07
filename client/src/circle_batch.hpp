#pragma once
#include <glm/glm.hpp>
#include "world.hpp"
#include "shader.hpp"
#include "gl_includes.hpp"

namespace Enflopio
{
    class CircleBatch
    {
    public:
        static CircleBatch& Instance()
        {
            static CircleBatch instance;
            return instance;
        }

        void SetProjection(const glm::mat4& projection)
        {
            m_projection = projection;
        }

        void SetView(const glm::mat4& view)
        {
            m_view = view;
        }

        void Draw(double delay);
        void Add(const World::Circle& circle);
        void Clear() { m_circles.clear(); }
    private:
        CircleBatch();
        Shader m_shader;
        GLuint m_vbo;

        glm::mat4 m_projection;
        glm::mat4 m_view;

        World::Circles m_circles;
    };
}
