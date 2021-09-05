#pragma once
#include "network_manager.hpp"
#include "messages.hpp"
#include "controls_state.hpp"
#include <deque>
#include <tuple>
#include <chrono>
#include <algorithm>
#include "generic_processes.hpp"
#include "interpolation_process.hpp"

namespace Enflopio
{
    //Class which responsible for sending and
    //processing outgoing and incoming player moves
    //Blame him for client player desync
    //
    //Intsdtst history is never empty
    class NetworkInputManager
    {
    public:
        NetworkInputManager(NetworkManager& manager, ProcessManager& process_manager);

        void ProcessControls(const ControlsState& controls, double delta)
        {
            assert(!m_history.empty());
            if (m_history.back().first.state != controls)
            {
                auto [id, timestamp] = NextIDTimestamp();
                NetworkControls new_input = {controls, id, timestamp};

                ServerMessages::Input message;
                message.delta = m_history.back().second;
                message.input = new_input;

                m_network.Send(Serialize(message));
                logging::net->debug("Sending input, id: {}, delta: {}", new_input.id, message.delta);

                m_history.push_back({new_input, 0});
            }

            m_history.back().second += delta;
        }

        void Receive(Player server_player, Player& client_player,
                     NetworkControls::ID server_ack_id, double input_delta,
                     InterpolationProcess& interpolation);
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
        ProcessManager& m_process_manager;
        NetworkControls m_last;
    };
}
