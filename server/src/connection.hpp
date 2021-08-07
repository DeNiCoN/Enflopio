#pragma once
#include <memory>
#include <span>
#include <vector>
#include <deque>
#include <shared_mutex>
#include <mutex>

namespace Enflopio
{
    class Connection {
    public:
        using Ptr = std::shared_ptr<Connection>;
        using Message = std::string;
        using MessageView = std::string_view;
        using Messages = std::deque<Message>;

        virtual ~Connection() = default;

        virtual void Send(Message message) = 0;
        virtual void Close() = 0;

        Message ReadNext()
        {
            std::scoped_lock lock(m_messages_mutex);
            auto message = std::move(m_received.front());
            m_received.pop_front();
            return message;
        }

        bool HasNext() const
        {
            std::shared_lock lock(m_messages_mutex);
            return m_received.size();
        };

    protected:
        void NewMessage(Message message)
        {
            std::scoped_lock lock(m_messages_mutex);
            m_received.push_back(std::move(message));
        }

    private:
        Messages m_received;
        mutable std::shared_mutex m_messages_mutex;
    };

}
