#include "network_input_manager.hpp"
#include "app.hpp"
#include "generic_processes.hpp"

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

        glm::vec2 delta_position = server_player.position - client_player.position;
        //Check/Update/Interpolate client_player
        if (std::abs(glm::length2(delta_position)) > 0.005 && !m_has_interpolation)
        {
            double interpolation_time = 1.0;
            glm::vec2 scaled = delta_position / static_cast<float>(interpolation_time);
            auto interpolate = Processes::Timed(interpolation_time,
                                                [&client_player, scaled](double delta)
                                                {
                                                    client_player.position += scaled * static_cast<float>(delta);
                                                });

            App::Instance().GetProcessManager().Add(ToPtr(std::move(interpolate)));

            m_has_interpolation = true;
        }
    }

}
