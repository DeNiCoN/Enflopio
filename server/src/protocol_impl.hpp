#pragma once
#include "connection.hpp"
#include "protocol.hpp"
#include "world.hpp"

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
    private:
        Connection& m_connection;
        World& m_world;
        World::PlayerID m_current_player;
    };
}
