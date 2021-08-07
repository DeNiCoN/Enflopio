#include "websocket_connection.hpp"

namespace Enflopio
{
    WebSocketConnection::WebSocketConnection(Socket* sock, uWS::Loop& loop)
        : m_socket(sock), m_loop(loop)
    {

    }

    void WebSocketConnection::Send(Message message)
    {
        m_loop.defer([&, message = std::move(message)]
        {
            m_socket->send(message);
        });
    }

    void WebSocketConnection::Close()
    {
        m_socket->getRemoteAddressAsText();
        m_socket->close();
    }
}
