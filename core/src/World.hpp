#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include "CirclePhysics.hpp"
#include "ControlsState.hpp"
//#include <ranges>
#include <memory>
#include <spdlog/spdlog.h>

namespace Enflopio
{
    class World
    {
    public:
        float PlayerVelocityCap = 1;

        using Circle = CirclePhysics::Circle;
        struct Player : public Circle
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

        using PlayerPtr = std::shared_ptr<Player>;

        using Circles = std::vector<Circle>;
        using Players = std::vector<PlayerPtr>;

        Circle& AddCircle(Circle circle)
        {
            m_circles.push_back(std::move(circle));
            return m_circles.back();
        }

        const Circles& GetCircles() const
        {
            return m_circles;
        }

        void AddPlayer(PlayerPtr player)
        {
            m_players.push_back(std::move(player));
        }

        const Players& GetPlayers() const
        {
            return m_players;
        }

        void Update(double delta)
        {
            for (auto& player : m_players)
            {
                player->Update(delta);
            }


            const auto as_circle = [](const auto& player) { return player.get(); };
            std::vector<Circle*> players;
            players.reserve(m_players.size());
            std::transform(m_players.begin(), m_players.end(), std::back_inserter(players), as_circle);

            physics.Proccess(m_circles, players, delta);
        }

    private:
        CirclePhysics physics;
        Circles m_circles;
        Players m_players;
    };
}
