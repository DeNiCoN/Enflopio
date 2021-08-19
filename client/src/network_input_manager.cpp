#include "network_input_manager.hpp"
#include "app.hpp"

namespace Enflopio
{
    void NetworkInputManager::Receive(Player server_player, Player& client_player,
                                      NetworkControls::ID server_ack_id, double input_delta)
    {
        App::Instance().ClearDebugDraw();
        App::Instance().AddDebugDraw(server_player.position, {0., 1., 0., 1.});
        //TODO
        //Clear acknowledged history
        ClearHistory(server_ack_id);
        spdlog::debug("Ack id: {}, history size: {}", server_ack_id, m_history.size());
        //Simulate server_player for history
        if (m_history.size() > 0)
            m_history.front().second -= input_delta;

        for (const auto& [move, delta] : m_history)
        {
            server_player.SetControls(move.state);
            double simulate = delta;
            while (simulate > 0)
            {
                server_player.Update(App::Instance().GetDesiredFps());
                simulate -= App::Instance().GetDesiredFps();
            }
        }
        //Check/Update/Interpolate client_player
        client_player.position = server_player.position;
        client_player.velocity = server_player.velocity;
        client_player.acceleration = server_player.acceleration;
    }

}
