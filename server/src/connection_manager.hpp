#pragma once
#include "websocket_connection_listener.hpp"
#include "tcp_connection_listener.hpp"

namespace Enflopio
{
    class ConnectionManager
    {
    public:
        ConnectionManager(std::uint16_t tcp_port, std::uint16_t websocket_port)
            : m_tcp_listener(tcp_port),
              m_websocket_listener(websocket_port)
        {}

        void StartListening()
        {
            m_tcp_listener.StartListening(
                [this]());
            m_websocket_listener.StartListening();
        }

        void StopListening()
        {
            m_websocket_listener.StopListening();
            m_tcp_listener.StopListening();
        }

        template<std::invocable<Connection::Ptr> NewCallback,
                 std::invocable<Connection::Ptr> DisconnectCallback>
        void ProcessConnections(NewCallback new_callback,
                                DisconnectCallback disconnect)
        {
            std::scoped_lock lock(m_connections_mutex);
            for (auto& ptr : m_pending_connect)
            {
                new_callback(std::move(ptr));
            }
            m_pending_connect.clear();

            for (auto& ptr : m_pending_disconnect)
            {
                disconnect(std::move(ptr));
            }
            m_pending_disconnect.clear();
        }

    private:
        void PendingConnect(Connection::Ptr ptr)
        {
            std::scoped_lock lock(m_connections_mutex);
            m_pending_connect.push_back(std::move(ptr));
        }

        void PendingDisconnect(Connection::Ptr ptr)
        {
            std::scoped_lock lock(m_connections_mutex);
            m_pending_disconnect.push_back(std::move(ptr));
        }

        std::mutex m_connections_mutex;
        std::vector<Connection::Ptr> m_pending_connect;
        std::vector<Connection::Ptr> m_pending_disconnect;
        TCPConnectionListener m_tcp_listener;
        WebSocketConnectionListener m_websocket_listener;

    };
}
