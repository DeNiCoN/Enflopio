#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <memory>

#include "circle_physics.hpp"
#include "controls_state.hpp"
#include "player.hpp"

namespace Enflopio
{
    class World
    {
    public:
        using Circle = CirclePhysics::Circle;


        using Circles = std::vector<Circle>;
        using PlayerID = std::size_t;
        using Players = std::unordered_map<PlayerID, Player>;

        Circle& AddCircle(Circle circle)
        {
            m_circles.push_back(std::move(circle));
            return m_circles.back();
        }

        const Circles& GetCircles() const
        {
            return m_circles;
        }

        PlayerID AddPlayer(Player player)
        {
            m_players[m_next_player_id] = std::move(player);
            return m_next_player_id++;
        }

        Player& GetPlayer(PlayerID id)
        {
            return m_players.at(id);
        }

        const Player& GetPlayer(PlayerID id) const
        {
            return m_players.at(id);
        }

        bool HasPlayer(PlayerID id) const
        {
            return m_players.count(id);
        }

        const Players& GetPlayers() const
        {
            return m_players;
        }

        void Update(double delta)
        {
            for (auto& [id, player] : m_players)
            {
                player.Update(delta);
            }


            const auto as_circle = [](const auto& player) { return player.second; };
            std::vector<Circle> players;
            players.reserve(m_players.size());
            std::transform(m_players.begin(), m_players.end(), std::back_inserter(players), as_circle);

            //TODO Rework
            m_physics.Proccess(m_circles, players, delta);
            m_physics.Move(m_circles, players);

            std::size_t i = 0;
            for (auto& [id, player] : m_players)
            {
                player = m_players[i];
            }
        }

    private:
        CirclePhysics m_physics;
        Circles m_circles;
        Players m_players;
        PlayerID m_next_player_id = 0;
    };
}
