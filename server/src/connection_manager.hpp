#pragma once
#include "websocket_connection_listener.hpp"
#include "tcp_connection_listener.hpp"

namespace Enflopio
{
    class ConnectionManager
    {
    public:
        ConnectionManager(std::uint16_t tcp_port, std::uint16_t websocket_port)
            : m_tcp_listener(tcp_port, *this),
              m_websocket_listener(websocket_port, *this)
        {}

        void StartListening()
        {
            m_tcp_listener.StartListening();
            m_websocket_listener.StartListening();
        }

        void StopListening()
        {
            m_websocket_listener.StopListening();
            m_tcp_listener.StopListening();
        }

        void PendingConnect(Connection::Ptr);
        void PendingDisconnect(Connection::Ptr);
    private:
        std::mutex m_connections_mutex;
        std::vector<Connection::Ptr> m_pending_connect;
        std::vector<Connection::Ptr> m_pending_disconnect;
        TCPConnectionListener m_tcp_listener;
        WebSocketConnectionListener m_websocket_listener;

    };
}
