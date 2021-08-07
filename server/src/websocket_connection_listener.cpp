#include "websocket_connection_listener.hpp"
#include <App.h>
#include <spdlog/spdlog.h>
#include "websocket_connection.hpp"
#include "server.hpp"
namespace Enflopio
{
    WebSocketConnectionListener::WebSocketConnectionListener(std::uint16_t port,
                                                             Server& server)
        : m_server(server), m_port(port)
    {
    }

    void WebSocketConnectionListener::StartListening()
    {
        m_listen_thread = std::thread([&]
        {
            spdlog::debug("Websocket server thread started");

            uWS::App().ws<WebSocketConnection::Ptr>("/*", {
                    .open = [&](auto* ws)
                    {
                        spdlog::info("New websocket connection {}",
                                     ws->getRemoteAddressAsText());
                        *ws->getUserData() = std::make_shared<WebSocketConnection>(ws, *uWS::Loop::get());
                        m_server.NewConnection(*ws->getUserData());
                    },
                    .message = [&](auto *ws, std::string_view message, uWS::OpCode opCode)
                    {
                        (*ws->getUserData())->NewMessage(std::string(message));
                    }
                }).listen(m_port, [&](auto *listen_socket)
                {
                    if (!listen_socket)
                    {
                        spdlog::critical("Failed to start listening websocket on {} port", m_port);
                        throw std::exception();
                    }
                    else
                    {
                        spdlog::info("Listening websocket on {} port started", m_port);
                    }
                }).run();
            spdlog::debug("Websocket server thread finished");
        });

    }

    void WebSocketConnectionListener::StopListening()
    {
        if (m_listen_thread.joinable())
        {
            m_listen_thread.join();
        }
    }
}
