#pragma once
#include "connection.hpp"
#include "protocol.hpp"
#include "world.hpp"
#include "network_controls.hpp"

namespace Enflopio
{
    class ProtocolImpl final : public ServerProtocol
    {
    public:
        ProtocolImpl(Connection& connection, World& world)
            : m_connection(connection), m_world(world)
        {

        }

        void Handle(const ServerMessages::Hello& msg) override;
        void Handle(const ServerMessages::Input& msg) override;

        void Disconnect();
        NetworkControls::ID LastInputId() const { return m_last_input_id; }
    private:
        Connection& m_connection;
        World& m_world;
        World::PlayerID m_current_player;
        NetworkControls::ID m_last_input_id;
    };
}
