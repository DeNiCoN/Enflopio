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
        m_app.m_current_player_id = m_app.m_world.AddPlayer(current_player);

        m_app.m_camera.SetPosition(m_app.m_world.GetPlayer(m_app.m_current_player_id).position);
    }

    void ProtocolImpl::Handle(const ClientMessages::Sync& msg)
    {
        m_app.m_debug_circles.clear();

        for (const auto& [id, player] : msg.players)
            m_app.m_debug_circles.push_back(player.position);
    }
}
