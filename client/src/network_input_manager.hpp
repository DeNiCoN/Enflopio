#pragma once
#include "network_manager.hpp"
#include "messages.hpp"
#include "controls_state.hpp"
#include <deque>
#include <tuple>
#include <chrono>
#include <algorithm>

namespace Enflopio
{
    //Class which responsible for sending and
    //processing outgoing and incoming player moves
    //Blame him for client player desync
    class NetworkInputManager
    {
    public:
        NetworkInputManager(NetworkManager& manager)
            : m_network(manager)
        {

        }

        void ProcessControls(const ControlsState& controls, double delta)
        {
            if (m_history.size() == 0 || m_history.back().first.state != controls)
            {
                auto [id, timestamp] = NextIDTimestamp();
                NetworkControls new_input = {controls, id, timestamp};
                m_history.push_back({new_input, delta});
                ServerMessages::Input message;
                message.input = new_input;
                m_network.Send(Serialize(message));
                spdlog::debug("Sending input, id: {}", new_input.id);
            }
            else
            {
                m_history.back().second += delta;
            }
        }

        void Receive(Player server_player, Player& client_player,
                     NetworkControls::ID server_ack_id, double input_delta);
    private:
        std::pair<NetworkControls::ID, NetworkControls::Timestamp> NextIDTimestamp()
        {
            return {m_next_id++, std::chrono::high_resolution_clock::now()};
        }

        void ClearHistory(NetworkControls::ID id)
        {
            auto it = std::lower_bound(m_history.begin(), m_history.end(), id,
                                       [](const auto& move, const auto& value)
                                       {
                                           return move.first.id < value;
                                       });

            m_history.erase(m_history.begin(), it);
        }

        NetworkControls::ID m_next_id = 0;
        std::deque<std::pair<NetworkControls, double>> m_history;
        NetworkManager& m_network;
        NetworkControls m_last;
        bool m_has_interpolation = false;
    };
}
