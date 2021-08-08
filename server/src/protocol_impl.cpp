#include "protocol_impl.hpp"
#include <spdlog/spdlog.h>
#include "messages.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

namespace Enflopio
{
    void ProtocolImpl::Handle(const ServerMessages::Hello &msg)
    {
        spdlog::debug("Hello received");
        glm::vec2 player_pos = glm::linearRand(glm::vec2(0., 0.), m_world.GetSize());
        Player new_player;
        new_player.position = player_pos;
        m_current_player = m_world.AddPlayer(new_player);

        ClientMessages::Hello message;
        message.player_pos = player_pos;
        spdlog::debug("Sending Hello: ({}, {})", player_pos.x, player_pos.y);
        m_connection.Send(Serialize(message));
    }

    void ProtocolImpl::Handle(const ServerMessages::Input &msg)
    {
        spdlog::debug("Input received");
    }
}
