#pragma once
#include <cstdint>
#include <thread>
#include "websocket_connection.hpp"
#include "utils/logging.hpp"

namespace Enflopio
{
    class WebSocketConnectionListener
    {
    public:
        WebSocketConnectionListener(std::uint16_t port)
            : m_port(port)
        {
        }

        ~WebSocketConnectionListener()
        {
            StopListening();
        }

        template<std::invocable<Connection::Ptr> NewCallback,
                 std::invocable<Connection::Ptr> DisconnectCallback>
        void StartListening(NewCallback n_callback,
                            DisconnectCallback d_callback)
        {
            m_listen_thread = std::thread([&]
            {
                logging::net->info("Websocket server thread started");

                uWS::App().ws<WebSocketConnection::Ptr>("/*", {
                        .open = [&](auto* ws)
                        {
                            logging::net->info("New websocket connection {}",
                                         ws->getRemoteAddressAsText());
                            *ws->getUserData() = std::make_shared<WebSocketConnection>(ws, *uWS::Loop::get());
                            n_callback(*ws->getUserData());
                        },
                        .message = [&](auto *ws, std::string_view message, uWS::OpCode opCode)
                        {
                            (*ws->getUserData())->NewMessage(std::string(message));
                        },
                        .close = [&](auto *ws, int code, std::string_view message)
                        {
                            d_callback(*ws->getUserData());
                        }
                    }).listen(m_port, [&](auto *listen_socket)
                    {
                        if (!listen_socket)
                        {
                            logging::net->critical("Failed to start listening websocket on {} port", m_port);
                            throw std::exception();
                        }
                        else
                        {
                            logging::net->info("Listening websocket on {} port started", m_port);
                        }
                    }).run();
                logging::net->info("Websocket server thread finished");
            });

        }

        void StopListening()
        {
            if (m_listen_thread.joinable())
            {
                m_listen_thread.join();
            }
        }
    private:
        std::uint16_t m_port;
        std::thread m_listen_thread;
    };
}
