#include "protocol_impl.hpp"
#include "utils/logging.hpp"
#include "messages.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

namespace Enflopio
{
    void ProtocolImpl::Disconnect()
    {
        if (m_world.HasPlayer(m_current_player))
            m_world.RemovePlayer(m_current_player);
    }

    void ProtocolImpl::Handle(const ServerMessages::Hello &msg)
    {
        logging::net->debug("Hello received");
        glm::vec2 player_pos = glm::linearRand(glm::vec2(0., 0.), m_world.GetSize());
        Player new_player;
        new_player.position = player_pos;
        m_current_player = m_world.AddPlayer(new_player);

        ClientMessages::Hello message;
        message.player_pos = player_pos;
        message.player_id = m_current_player;
        logging::net->debug("Sending Hello: ({}, {})", player_pos.x, player_pos.y);
        m_connection.Send(Serialize(message));
    }

    void ProtocolImpl::Handle(const ServerMessages::Input &msg)
    {
        logging::net->debug("Input received, Player: {}, id: {}, delta: {}", m_current_player, msg.input.id, msg.delta);
        m_history.push_back({msg.input.state, msg.delta});
        if (m_history.size() == 1)
            m_world.GetPlayer(m_current_player).SetControls(m_history[1].first);
    }
}
