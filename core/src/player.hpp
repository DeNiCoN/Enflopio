#pragma once
#include "circle_physics.hpp"
#include "controls_state.hpp"
#include "utils/logging.hpp"
#include "utils/glm_operators.hpp"

namespace Enflopio
{
    struct Player : public CirclePhysics::Circle
    {
    public:
        static constexpr float VELOCITY_CAP = 8;
        static constexpr float ACCELERATION = 30;
        static constexpr float DAMP = 30;

        Player()
        {
            radius = 2;
        }

        void Update(double delta)
        {
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
            logging::net->debug("Player controls: {}", controls);
        }

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<CirclePhysics::Circle>(this), m_controls);
        }

    private:
        ControlsState m_controls;
    };
}

inline std::ostream& operator<<(std::ostream& out, const Enflopio::Player& rhs)
{
    return out << "(" << rhs.position.x << ", " << rhs.position.y << ")";
}
