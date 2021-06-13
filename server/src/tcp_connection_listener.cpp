#include "tcp_connection_listener.hpp"
#include "server.hpp"
#include <boost/asio.hpp>
#include "tcp_connection.hpp"
#include <iostream>

namespace Enflopio
{
    void TCPConnectionListener::StartListening()
    {
        m_listen_thread = std::thread([&]
        {
            StartAccept();
            m_io_service.run();
        });
    }

    void TCPConnectionListener::StopListening()
    {
        m_io_service.stop();
        m_listen_thread.join();
    }

    void TCPConnectionListener::StartAccept()
    {
        m_acceptor.async_accept([&](const boost::system::error_code& error,
                                    boost::asio::ip::tcp::socket socket)
        {
            if (!error)
            {
                m_server.NewConnection(std::make_shared<TCPConnection>(std::move(socket)));
            }
            StartAccept();
        });
    }
}
