#include "network_manager.hpp"

namespace Enflopio
{
    void NetworkManager::Init()
    {
#ifdef __EMSCRIPTEN__
        short port = 25326;
        const auto& address = "93.74.188.74";
        m_connection.Connect("ws://93.74.188.74:25326");
#else
        short port = 25325;
        const auto& address = "93.74.188.74";
        m_connection.Connect("93.74.188.74", 25325);
#endif

    }

}
