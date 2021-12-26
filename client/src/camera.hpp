#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Enflopio
{
    class Camera
    {
    public:
        void SetBorders(float width, float height)
        {
            auto half_width = width/2;
            auto half_height = height/2;
            m_projection = glm::ortho(-half_width, half_width,
                                      -half_height, half_height);
        }

        void SetScale(float scale)
        {
            m_scale = scale;
        }

        void Move(glm::vec2 position)
        {
            m_position = position;
        }

        void SetPosition(glm::vec2 position)
        {
            m_current_position = position;
            m_position = position;
        }

        void Update(double delta)
        {

            glm::vec2 dir = (m_position - m_current_position);
            if (glm::length2(dir) > 0)
                m_current_position += glm::length2(dir) * glm::normalize(dir) * static_cast<float>(delta);
        }

        const glm::mat4& GetProjection() const
        {
            return m_projection;
        }

        glm::mat4 CalculateView(double delay) const
        {
            glm::mat4 result = glm::identity<glm::mat4>();
            result = glm::scale(result, glm::vec3(m_scale));
            result = glm::translate(result, glm::vec3(-m_current_position, 0.0));
            return result;
        }
    private:
        glm::mat4 m_projection;
        glm::vec2 m_current_position;
        glm::vec2 m_position;
        float m_scale;
    };
}
