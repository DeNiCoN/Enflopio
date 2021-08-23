#pragma once
#include "process_manager.hpp"
#include "world.hpp"

namespace Enflopio
{
    class InterpolationProcess : public Process
    {
    public:
        InterpolationProcess(World& world, World::PlayerID id)
            : m_world(world), m_player_id(id)
        {

        }

        void Update(double delta) override
        {
            if (m_world.HasPlayer(m_player_id))
            {
                auto& player = m_world.GetPlayer(m_player_id);
                if (glm::length2(offset) > 0)
                {
                    float fdelta = delta;
                    auto normalized = glm::normalize(offset);
                    auto static_v = normalized * fdelta * speed;
                    auto dynamic_v = offset * fdelta * speed;

                    player.position += static_v + dynamic_v;
                    offset -= static_v + dynamic_v;

                    if (glm::dot(offset, normalized) <= 0)
                    {
                        player.position += offset;
                        offset = {0.f, 0.f};
                    }
                }
            }
        }

        float speed = 1.f;
        glm::vec2 offset = {0.f, 0.f};
    private:
        World& m_world;
        World::PlayerID m_player_id;
    };
}
