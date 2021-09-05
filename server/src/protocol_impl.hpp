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
            m_history.push_back({{}, 0});
        }

        void Handle(const ServerMessages::Hello& msg) override;
        void Handle(const ServerMessages::Input& msg) override;

        void UpdateMove(double delta)
        {
            m_current_input_delta += delta;
            if (m_history.size() >= 2)
            {
                if (m_current_input_delta >= m_history[1].second)
                {
                    m_current_input_delta = m_current_input_delta - m_history[1].second;
                    m_last_input_delta = m_last_input_delta - m_history[1].second;
                    m_world.GetPlayer(m_current_player).SetControls(m_history[1].first);
                    m_last_input_id++;
                    m_history.pop_front();
                    ResetMove();
                }
            }
        }

        void ResetMove()
        {
            m_last_input_delta = m_current_input_delta;
        }

        void Disconnect();
        NetworkControls::ID LastInputId() const { return m_last_input_id; }
        double InputDelta() const { return m_current_input_delta - m_last_input_delta; }
    private:
        Connection& m_connection;
        World& m_world;
        World::PlayerID m_current_player;
        NetworkControls::ID m_last_input_id = 0;
        double m_last_input_delta = 0.;
        double m_current_input_delta = 0.;
        std::deque<std::pair<ControlsState, double>> m_history;
    };
}
