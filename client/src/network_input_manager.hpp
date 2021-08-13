#pragma once
#include "network_manager.hpp"
#include "messages.hpp"
#include "controls_state.hpp"
#include <deque>

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

        void ProcessControls(const ControlsState& controls)
        {
            if (m_sent.size() == 0 || m_sent.back() != controls)
            {
                m_sent.push_back(controls);
                ServerMessages::Input message;
                message.input = controls;

                m_network.Send(Serialize(message));
            }
        }

    private:
        std::deque<ControlsState> m_sent;
        NetworkManager& m_network;
    };
}
