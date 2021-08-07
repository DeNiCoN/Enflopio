#pragma once
#include <string>
#include <boost/asio.hpp>
#include <deque>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <optional>

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

        void Connect(std::string_view address, short port);

        void Send(Message message);

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
        void ReadNextMessage();
        void ReadMessage(uint32_t size);
        void Write();

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
