#pragma once
#include <string>
#include <deque>
#include <cereal/archives/portable_binary.hpp>
#include <sstream>
#include "messages.hpp"

#ifdef __EMSCRIPTEN__
#include "websocket_connection.hpp"
#else
#include "asio_connection.hpp"
#endif

namespace Enflopio
{
    class NetworkConnection
    {
    public:
        using Message = std::string;
        using MessageView = std::string_view;
        using Messages = std::deque<Message>;

        void Connect(std::string_view address, short port)
        {
            m_connection.Connect(address, port);
        }

        Message NextMessage()
        {
            return m_connection.NextMessage();
        }

        bool HasNextMessage() const
        {
            return m_connection.HasNextMessage();
        }

        void Send(Message message)
        {
            m_connection.Send(std::move(message));
        }

    private:
#ifdef __EMSCRIPTEN__
        using Connection = WebsocketConnection;
#else
        using Connection = ASIOConnection;
#endif

        Connection m_connection;
    };

    class NetworkManager
    {
    public:
        void Connect(std::string_view address, short port)
        {
            m_connection.Connect(address, port);
        }

        ClientMessage::Ptr NextMessage()
        {
            return ClientMessages::Deserialize(m_connection.NextMessage());
        }

        bool HasNextMessage() const
        {
            return m_connection.HasNextMessage();
        }

        void Send(const ServerMessage& message)
        {
            m_connection.Send(ServerMessages::Serialize(message));
        }
    private:
        NetworkConnection m_connection;
    };
}
