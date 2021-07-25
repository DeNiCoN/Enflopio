#pragma once
#include <cstdint>
#include <thread>

namespace Enflopio
{
    class Server;
    class WebSocketConnectionListener
    {
    public:
        WebSocketConnectionListener(std::uint16_t port, Server& server);
        ~WebSocketConnectionListener()
        {
            StopListening();
        }
        void StartListening();
        void StopListening();
    private:
        Server& m_server;
        std::uint16_t m_port;
        std::thread m_listen_thread;
    };
}
