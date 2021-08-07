#pragma once
#include "connection.hpp"
#include "App.h"

namespace Enflopio
{
    class WebSocketConnectionListener;
    class WebSocketConnection : public Connection
    {
    public:
        using Ptr = std::shared_ptr<WebSocketConnection>;
        using Socket = uWS::WebSocket<false, true, Ptr>;

        WebSocketConnection(Socket* sock, uWS::Loop& loop);

        void Send(Message span) override;
        void Close() override;
    private:
        Socket* m_socket;
        uWS::Loop& m_loop;

        friend class WebSocketConnectionListener;
    };
}
