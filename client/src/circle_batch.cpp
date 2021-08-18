#include "circle_batch.hpp"
#include "shader_loader.hpp"

namespace Enflopio
{
    static const struct
    {
        float x, y;
    } vertices[6] =
    {
        {-0.5f, 0.5f},
        {-0.5f, -0.5f},
        {0.5f, -0.5f},

        {-0.5f, 0.5f},
        {0.5f, -0.5f},
        {0.5f, 0.5f}
    };

    static const char* vertex_shader_text = R"(
attribute vec2 vert_position;

uniform float radius;
uniform vec2 position;
uniform mat4 view;
uniform mat4 projection;

varying vec2 uv;
void main()
{
   uv = vert_position * 2.0;
   gl_Position = projection * view *
       (vec4(radius * vert_position.x, radius * vert_position.y, 0, 1) + vec4(position, 0, 0));
})";

    static const char* fragment_shader_text = R"(
precision mediump float;
uniform vec4 color;

varying vec2 uv;

void main()
{
    if (dot(uv, uv) > 1.0) discard;

    gl_FragColor = color;
}
)";

    CircleBatch::CircleBatch()
        : m_shader(ShaderLoader::FromText(vertex_shader_text, fragment_shader_text))
    {
        m_shader.Use();

        glGenBuffers(1, &m_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }

    void CircleBatch::Add(const World::Circle& circle, glm::vec4 color)
    {
        m_circles.push_back(std::make_pair(circle, color));
    }

    void CircleBatch::Draw(double delay)
    {
        m_shader.Use();
        m_shader.SetUniform("projection", m_projection);
        m_shader.SetUniform("view", m_view);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                              sizeof(vertices[0]), (void *)0);
        glEnableVertexAttribArray(0);

        for (const auto& [circle, color] : m_circles)
        {
            m_shader.SetUniform("radius", circle.radius);
            m_shader.SetUniform("position", circle.position);
            m_shader.SetUniform("color", color);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
    }
}
