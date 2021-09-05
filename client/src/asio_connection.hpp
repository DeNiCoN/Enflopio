#pragma once
#include <string>
#include <boost/asio.hpp>
#include <deque>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <optional>
#include "utils/logging.hpp"

namespace Enflopio
{
    class ASIOConnection
    {
    public:
        using Message = std::string;
        using MessageView = std::string_view;
        using Messages = std::deque<Message>;

        ASIOConnection() : m_socket(m_io_context)
        {

        }

        ~ASIOConnection()
        {
            m_io_context.stop();
            if (m_messages_thread.joinable())
            {
                m_messages_thread.join();
            }
        }

        void Connect(std::string_view address, short port)
        {
            logging::net->info("ASIOConnection resolving endpoints {}:{}", address, port);
            boost::asio::ip::tcp::resolver resolver(m_io_context);
            auto endpoints = resolver.resolve(address, std::to_string(port));

            logging::net->info("{} endpoints resolved. Connecting...", endpoints.size());
            boost::asio::connect(m_socket, endpoints);

            logging::net->info("Connection successfull", endpoints.size());

            ReadNextMessage();
            m_messages_thread = std::thread([&] {
                m_io_context.run();
                logging::net->info("IO context stopped");
            });
        }

        void Send(Message message)
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

        Message NextMessage()
        {
            std::scoped_lock lock(m_messages_mutex);
            auto result = std::move(m_input_messages.front());
            m_input_messages.pop_front();
            return result;
        }

        bool HasNextMessage() const
        {
            std::shared_lock lock(m_messages_mutex);
            return m_input_messages.size();
        }

    private:
        void ReadNextMessage()
        {
            boost::asio::async_read(
                m_socket,
                boost::asio::buffer(&m_current_input_header, sizeof(m_current_input_header)),
                [this](auto error_code, std::size_t)
                {
                    if (error_code)
                    {
                        logging::net->error("Could not read header: {}",
                                      boost::system::system_error(error_code).what());
                        return;
                    }
                    else
                    {
                        logging::net->debug("Read message, size {}", m_current_input_header);
                        ReadMessage(m_current_input_header);
                    }
                });
        }

        void ReadMessage(uint32_t size)
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
                        logging::net->error("Could not read message: {}",
                                      boost::system::system_error(error_code).what());
                        return;
                    }
                    else
                    {
                        {
                            std::scoped_lock lock(m_messages_mutex);
                            m_input_messages.push_back(std::move(m_current_message));
                        }
                        ReadNextMessage();
                    }
                }
                );
        }

        void Write()
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
                        logging::net->error("Could not write: {}",
                                      boost::system::system_error(error_code).what());
                        return;
                    }

                    if (!m_output_messages.empty())
                    {
                        Write();
                    }
                });
        }

        boost::asio::io_context m_io_context;
        boost::asio::ip::tcp::socket m_socket;
        Messages m_input_messages;
        Messages m_output_messages;
        mutable std::shared_mutex m_messages_mutex;
        std::thread m_messages_thread;

        std::uint32_t m_current_input_header;
        std::uint32_t m_current_output_header;
        Message m_current_message;
    };
}
