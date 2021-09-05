#include "protocol_impl.hpp"
#include "app.hpp"
#include <spdlog/spdlog.h>
#include "messages.hpp"
#include <tuple>

namespace Enflopio
{
    void ProtocolImpl::Handle(const ClientMessages::Hello &msg)
    {
        logging::net->debug("Hello received: ({}, {})", msg.player_pos.x, msg.player_pos.y);
        Player current_player;
        current_player.position = msg.player_pos;
        m_app.m_current_player_id = m_app.m_world.AddPlayer(current_player, msg.player_id);

        m_app.m_camera.SetPosition(m_app.m_world.GetPlayer(m_app.m_current_player_id).position);

        m_app.m_interpolations.try_emplace(m_app.m_current_player_id, m_app.m_world, m_app.m_current_player_id);
    }

    void ProtocolImpl::Handle(const ClientMessages::Sync& msg)
    {
        if (m_app.m_world.HasPlayer(m_app.m_current_player_id))
        {
            Player backup = m_app.m_world.m_players.at(m_app.m_current_player_id);
            m_app.m_world.m_players = msg.players;
            m_app.m_world.m_players.at(m_app.m_current_player_id) = backup;

            auto id = m_app.m_current_player_id;
            m_app.m_network_input.Receive(msg.players.at(id), m_app.m_world.GetPlayer(id),
                                          msg.last_input_id, msg.last_input_delta,
                                          m_app.m_interpolations.at(id));

            m_app.m_world.m_circles = msg.circles;
            logging::net->debug("Sync, circles: {}", msg.circles.size());
        }
    }
}
