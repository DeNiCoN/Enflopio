#pragma once
#include "CirclePhysics.hpp"
#include "ControlsState.hpp"

namespace Enflopio
{
    struct Player : public CirclePhysics::Circle
    {
    public:
        const float VELOCITY_CAP = 8;
        const float ACCELERATION = 30;
        const float DAMP = 30;

        Player()
        {
            radius = 2;
        }

        void Update(double delta)
        {
            float speed = glm::length(velocity);
            velocity += m_controls.direction * ACCELERATION * static_cast<float>(delta);
            if (glm::length2(velocity) >= VELOCITY_CAP*VELOCITY_CAP)
                velocity = glm::normalize(velocity) * VELOCITY_CAP;

            if (glm::length(m_controls.direction) == 0 && glm::length2(velocity) != 0)
            {
                glm::vec2 dump_vec = glm::normalize(velocity) * DAMP * static_cast<float>(delta);
                if (glm::length2(dump_vec) > glm::length2(velocity))
                    velocity = {0, 0};
                else
                    velocity -= dump_vec;
            }

            position += velocity * static_cast<float>(delta);
        }

        void SetControls(const ControlsState& controls)
        {
            m_controls = controls;
        }

    private:
        ControlsState m_controls;
    };
}
