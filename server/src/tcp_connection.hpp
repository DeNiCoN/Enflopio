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
        using IOContext = boost::asio::io_context;

        TCPConnection(Socket socket, IOContext& io_context);

        Socket& GetSocket() { return m_socket; }

        void Send(Message message) override;
        void Close() override;
    private:
        void ReadNextMessage();
        void ReadMessage(uint32_t size);
        void Write();

        Socket m_socket;
        boost::asio::io_context& m_io_context;
        Messages m_input_messages;
        Messages m_output_messages;
        std::thread m_messages_thread;

        std::uint32_t m_current_input_header;
        std::uint32_t m_current_output_header;
        Message m_current_message;

        friend class TCPConnectionListener;
    };
}
