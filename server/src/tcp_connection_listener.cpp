#include "tcp_connection_listener.hpp"
#include "server.hpp"
#include <boost/asio.hpp>
#include "tcp_connection.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

namespace Enflopio
{
    void TCPConnectionListener::StartListening()
    {
        m_listen_thread = std::thread([&]
        {
            spdlog::debug("TCP server thread started");
            StartAccept();
            m_io_service.run();
        });
    }

    void TCPConnectionListener::StopListening()
    {
        m_io_service.stop();
        if (m_listen_thread.joinable())
        {
            m_listen_thread.join();
        }
    }

    void TCPConnectionListener::StartAccept()
    {
        spdlog::info("Listening tcp on {} port started", m_port);
        m_acceptor.async_accept([&](const boost::system::error_code& error,
                                    boost::asio::ip::tcp::socket socket)
        {
            if (!error)
            {
                spdlog::info("New tcp connection");
                auto connection = std::make_shared<TCPConnection>(std::move(socket), m_io_service,
                                                                  [&](Connection::Ptr ptr)
                                                                  {
                                                                      m_server.PendingDisconnect(std::move(ptr));
                                                                  });
                connection->ReadNextMessage();
                m_server.PendingConnect(std::move(connection));
            }
            else
            {
                spdlog::info("Error listening tcp connection: {}", error.message());
            }
            StartAccept();
        });
    }
}
