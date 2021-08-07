#include "tcp_connection.hpp"
#include "tcp_connection_listener.hpp"
#include <spdlog/spdlog.h>

namespace Enflopio
{
    TCPConnection::TCPConnection(Socket socket, IOContext& io_context)
        : m_socket(std::move(socket)), m_io_context(io_context)
    {

    }

    void TCPConnection::Send(Message message)
    {
        m_io_context.post(
            [this, message = std::move(message)]()
            {
                m_output_messages.push_back(std::move(message));
                if (m_output_messages.size() > 1)
                    return;

                Write();
            });
    }

    void TCPConnection::Close()
    {
        m_socket.close();
    }

    void TCPConnection::ReadNextMessage()
    {
        boost::asio::async_read(
            m_socket,
            boost::asio::buffer(&m_current_input_header, sizeof(m_current_input_header)),
            [this](auto error_code, std::size_t)
            {
                spdlog::debug("New tcp header");
                if (error_code)
                {
                    spdlog::error("Could not read header: {}",
                                  boost::system::system_error(error_code).what());
                    return;
                }
                else
                {
                    ReadMessage(m_current_input_header);
                }
            });
    }

    void TCPConnection::ReadMessage(uint32_t size)
    {
        m_current_message.resize(size);
        boost::asio::async_read(
            m_socket,
            boost::asio::buffer(m_current_message.data(),
                                m_current_message.size()),
            [this](auto error_code, std::size_t)
            {
                spdlog::debug("New tcp message");
                if (error_code)
                {
                    spdlog::error("Could not read message: {}",
                                  boost::system::system_error(error_code).what());
                    return;
                }
                else
                {
                    NewMessage(std::move(m_current_message));
                    ReadNextMessage();
                }
            }
            );
    }

    void TCPConnection::Write()
    {
        const auto& message = m_output_messages.front();
        m_current_output_header = message.size();

        std::array<boost::asio::const_buffer, 2> full_message;
        full_message[0] = boost::asio::buffer(&m_current_output_header,
                                              sizeof(m_current_output_header));
        full_message[1] = boost::asio::buffer(message.data(), message.size());
        boost::asio::async_write(
            m_socket,
            full_message,
            [this](auto error_code, std::size_t)
            {
                m_output_messages.pop_front();

                if (error_code)
                {
                    spdlog::error("Could not write: {}",
                                  boost::system::system_error(error_code).what());
                    return;
                }

                if (!m_output_messages.empty())
                {
                    Write();
                }
            });
    }
}
