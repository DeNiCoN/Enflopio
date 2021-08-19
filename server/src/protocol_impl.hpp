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

        void UpdateMove(double delta)
        {
            m_last_input_delta += delta;
        }

        void ResetMove()
        {
            m_last_input_delta = 0;
        }

        void Disconnect();
        NetworkControls::ID LastInputId() const { return m_last_input_id; }
        double LastInputDelta() const { return m_last_input_delta; }
    private:
        Connection& m_connection;
        World& m_world;
        World::PlayerID m_current_player;
        NetworkControls::ID m_last_input_id;
        double m_last_input_delta = 0.;
    };
}
