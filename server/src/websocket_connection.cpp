#include "websocket_connection.hpp"

namespace Enflopio
{
    WebSocketConnection::WebSocketConnection(Socket* sock)
        : m_socket(sock)
    {

    }

    void WebSocketConnection::Send(std::span<const char> span)
    {
        m_socket->send(std::string_view(span.data(), span.size()));
    }

    void WebSocketConnection::Close()
    {
        m_socket->getRemoteAddressAsText();
        m_socket->close();
    }
}
