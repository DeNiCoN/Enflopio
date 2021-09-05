#pragma once
#include <cstdint>
#include <boost/asio.hpp>
#include <thread>
#include "tcp_connection.hpp"
#include "utils/logging.hpp"

namespace Enflopio
{
    class ConnectionManager;
    class TCPConnectionListener
    {
    public:
        explicit TCPConnectionListener(std::uint16_t port)
            : m_acceptor(m_io_service,
                         boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
              m_port(port)
        {
        }

        ~TCPConnectionListener()
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
                logging::net->info("TCP server thread started");
                StartAccept(std::move(n_callback), std::move(d_callback));
                m_io_service.run();
            });
        }

        void StopListening()
        {
            m_io_service.stop();
            if (m_listen_thread.joinable())
            {
                m_listen_thread.join();
            }
        }
    private:
        template<std::invocable<Connection::Ptr> NewCallback,
                 std::invocable<Connection::Ptr> DisconnectCallback>
        void StartAccept(NewCallback n_callback, DisconnectCallback d_callback)
        {
            logging::net->info("Listening tcp on {} port started", m_port);
            m_acceptor.async_accept([this,
                                     n_callback = std::move(n_callback),
                                     d_callback = std::move(d_callback)]
                                    (const boost::system::error_code& error,
                                     boost::asio::ip::tcp::socket socket)
            {
                if (!error)
                {
                    logging::net->info("New tcp connection");
                    auto connection =
                        std::make_shared<TCPConnection>(
                            std::move(socket), m_io_service,
                            [d_callback](Connection::Ptr ptr)
                            {
                                d_callback(std::move(ptr));
                            });
                    connection->ReadNextMessage();
                    n_callback(std::move(connection));
                }
                else
                {
                    logging::net->error("Error listening tcp connection: {}", error.message());
                }
                StartAccept(std::move(n_callback), std::move(d_callback));
            });
        }

        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;
        std::uint16_t m_port;
        std::thread m_listen_thread;

        friend class TCPConnection;
    };
}
