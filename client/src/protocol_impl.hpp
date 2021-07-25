#pragma once
#include "Protocol.hpp"
#include "App.hpp"

namespace Enflopio
{
    class ProtocolImpl final : public ClientProtocol
    {
    public:
        void Handle(const ClientMessages::Hello& msg)
        {
            Player current_player = Player();
            current_player.position = {0, 0};
            current_player.velocity = {0, 0};
            app.m_current_player_id = app.m_world.AddPlayer(current_player);

            for (int i = 0; i < 100; i++)
            {
                app.m_world.AddCircle({{rand() % 100 - 50, rand() % 100 - 50}});
            }
        }
    private:
        App& app;
    };
}
