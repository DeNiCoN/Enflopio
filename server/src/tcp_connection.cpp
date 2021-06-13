#include "tcp_connection.hpp"
#include "tcp_connection_listener.hpp"

namespace Enflopio
{
    TCPConnection::TCPConnection(Socket socket)
        : m_socket(std::move(socket))
    {
        //boost::asio::async_read TODO
    }

    void TCPConnection::Send(std::span<const char> span)
    {
        boost::asio::async_write(m_socket, boost::asio::buffer(span.data(), span.size()),
                                 [&, connection = shared_from_this()]
                                 (const boost::system::error_code& error,
                                  std::size_t bytes_transfered)
                                 {
                                 });
    }

    void TCPConnection::Close()
    {
        m_socket.close();
    }
}
