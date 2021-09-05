#include "network_input_manager.hpp"
#include "app.hpp"
#include "generic_processes.hpp"

namespace Enflopio
{

    NetworkInputManager::NetworkInputManager(NetworkManager& manager, ProcessManager& process_manager)
        : m_network(manager), m_process_manager(process_manager)
    {
        auto [id, timestamp] = NextIDTimestamp();
        NetworkControls new_input = {{}, id, timestamp};
        m_history.push_back({new_input, 0});
    }
    void NetworkInputManager::Receive(Player server_player, Player& client_player,
                                      NetworkControls::ID server_ack_id, double input_delta,
                                      InterpolationProcess& interpolation)
    {
        App::Instance().ClearDebugDraw();
        App::Instance().AddDebugDraw(server_player.position, {0., 1., 0., 1.});
        //TODO
        //Clear acknowledged history
        ClearHistory(server_ack_id);
        logging::net->debug("Ack id: {}, history size: {}", server_ack_id, m_history.size());
        //Simulate server_player for history
        m_history.front().second -= input_delta;

        for (const auto& [move, delta] : m_history)
        {
            server_player.SetControls(move.state);
            double simulate = delta;
            while (simulate > 0)
            {
                server_player.Update(App::Instance().GetDesiredDelta());
                simulate -= App::Instance().GetDesiredDelta();
            }
        }

        m_history.front().second += input_delta;

        glm::vec2 delta_position = server_player.position - client_player.position;
        //Check/Update/Interpolate client_player
        if (std::abs(glm::length2(delta_position)) > 0.0005)
        {
            interpolation.offset = delta_position;
        }
    }
}
