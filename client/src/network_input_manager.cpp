#include "network_input_manager.hpp"
#include "app.hpp"

namespace Enflopio
{
    void NetworkInputManager::Receive(Player server_player, Player& client_player, NetworkControls::ID server_ack_id)
    {
        App::Instance().ClearDebugDraw();
        App::Instance().AddDebugDraw(server_player.position, {0., 1., 0., 1.});
        //TODO
        //Clear acknowledged history
        ClearHistory(server_ack_id);
        spdlog::debug("Ack id: {}", server_ack_id);
        //Simulate server_player for history
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
        App::Instance().AddDebugDraw(server_player.position, {1., 0., 0., 1.});
    }

}
