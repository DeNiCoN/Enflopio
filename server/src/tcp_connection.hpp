#pragma once
#include <boost/asio.hpp>
#include "connection.hpp"

namespace Enflopio
{
    class TCPConnectionListener;
    class TCPConnection : public Connection,
                          public std::enable_shared_from_this<TCPConnection>
    {
    public:
        using Ptr = std::shared_ptr<TCPConnection>;
        using Socket = boost::asio::ip::tcp::socket;

        TCPConnection(Socket socket);

        Socket& GetSocket() { return m_socket; }

        void Send(std::span<const char> span) override;
        void Close() override;
    private:
        Socket m_socket;
    };
}
