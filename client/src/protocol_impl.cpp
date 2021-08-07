#include "protocol_impl.hpp"
#include "app.hpp"
#include <spdlog/spdlog.h>

namespace Enflopio
{
    void ProtocolImpl::Handle(const ClientMessages::Hello &msg)
    {
        spdlog::info("Hello received");
        Player current_player = Player();
        current_player.position = {0, 0};
        current_player.velocity = {0, 0};
        m_app.m_current_player_id = m_app.m_world.AddPlayer(current_player);
        m_app.m_camera.SetPosition(m_app.m_world.GetPlayer(m_app.m_current_player_id).position);

        for (int i = 0; i < 100; i++)
        {
            m_app.m_world.AddCircle({{rand() % 100 - 50, rand() % 100 - 50}});
        }
    }
}
