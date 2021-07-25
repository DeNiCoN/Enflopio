#pragma once
#include <cstdint>
#include <boost/asio.hpp>
#include <thread>
#include "tcp_connection.hpp"

namespace Enflopio
{
    class Server;
    class TCPConnectionListener
    {
    public:
        TCPConnectionListener(std::uint16_t port, Server& server)
            : m_server(server),
              m_acceptor(m_io_service,
                         boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
        {
        }

        ~TCPConnectionListener()
        {
            StopListening();
        }

        void StartListening();
        void StopListening();
    private:
        void StartAccept();
        void HandleWrite(TCPConnection::Ptr connection,
                         const boost::system::error_code& error);
        void HandleReceive(TCPConnection::Ptr connection,
                           const boost::system::error_code& error);

        Server& m_server;
        boost::asio::io_service m_io_service;
        boost::asio::ip::tcp::acceptor m_acceptor;
        std::thread m_listen_thread;

        friend class TCPConnection;
    };
}
