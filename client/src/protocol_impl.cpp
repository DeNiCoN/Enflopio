#include "protocol_impl.hpp"
#include "app.hpp"
#include <spdlog/spdlog.h>
#include "messages.hpp"

namespace Enflopio
{
    void ProtocolImpl::Handle(const ClientMessages::Hello &msg)
    {
        spdlog::debug("Hello received: ({}, {})", msg.player_pos.x, msg.player_pos.y);
        Player current_player;
        current_player.position = msg.player_pos;
        m_app.m_current_player_id = m_app.m_world.AddPlayer(current_player, msg.player_id);

        m_app.m_camera.SetPosition(m_app.m_world.GetPlayer(m_app.m_current_player_id).position);
    }

    void ProtocolImpl::Handle(const ClientMessages::Sync& msg)
    {
        for (const auto& [id, player] : msg.players)
        {
            if (id != m_app.m_current_player_id)
            {
                if (!m_app.m_world.HasPlayer(id))
                    m_app.m_world.AddPlayer(player, id);
                else
                    m_app.m_world.GetPlayer(id) = player;
            }
            else
            {
                m_app.m_network_input.Receive(player, m_app.m_world.GetPlayer(id), msg.last_input_id);
            }
        }
    }
}
