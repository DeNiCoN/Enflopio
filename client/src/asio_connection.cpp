#include "asio_connection.hpp"
#include <spdlog/spdlog.h>
#include <array>
#include <cinttypes>

namespace Enflopio
{
    using namespace boost::asio::ip;
    void ASIOConnection::Connect(std::string_view address, short port)
    {
        spdlog::debug("ASIOConnection resolving endpoints {}:{}", address, port);
        tcp::resolver resolver(m_io_context);
        auto endpoints = resolver.resolve(address, std::to_string(port));

        spdlog::debug("{} endpoints resolved. Connecting...", endpoints.size());
        boost::asio::connect(m_socket, endpoints);

        spdlog::info("Connection successfull", endpoints.size());

        ReadNextMessage();
        m_messages_thread = std::thread([&] {m_io_context.run();});
    }

    void ASIOConnection::ReadNextMessage()
    {
        boost::asio::async_read(
            m_socket,
            boost::asio::buffer(&m_current_input_header, sizeof(m_current_input_header)),
            [this](auto error_code, std::size_t)
            {
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

    void ASIOConnection::ReadMessage(uint32_t size)
    {
        m_current_message.resize(size);
        boost::asio::async_read(
            m_socket,
            boost::asio::buffer(m_current_message.data(),
                                m_current_message.size()),
            [this](auto error_code, std::size_t)
            {
                if (error_code)
                {
                    spdlog::error("Could not read message: {}",
                                  boost::system::system_error(error_code).what());
                    return;
                }
                else
                {
                    {
                        std::lock_guard lock(m_messages_mutex);
                        m_input_messages.push_back(std::move(m_current_message));
                    }

                    ReadNextMessage();
                }
            }
            );
    }

    void ASIOConnection::Send(Message message)
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

    void ASIOConnection::Write()
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
