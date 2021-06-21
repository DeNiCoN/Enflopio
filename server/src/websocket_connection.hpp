#pragma once
#include "connection.hpp"
#include "App.h"

namespace Enflopio
{
    class WebSocketConnection : public Connection
    {
    public:
        using Ptr = std::shared_ptr<WebSocketConnection>;
        using Socket = uWS::WebSocket<false, true, Ptr>;

        WebSocketConnection(Socket* sock);

        void Send(std::span<const char> span) override;
        void Close() override;
    private:
        Socket* m_socket;

    };
}
