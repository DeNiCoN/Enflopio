#pragma once
#include <memory>
#include <span>
#include <vector>
#include <deque>

namespace Enflopio
{
    class Connection {
    public:
        using Ptr = std::shared_ptr<Connection>;
        using Message = std::string;
        using Messages = std::deque<Message>;

        virtual ~Connection() = default;

        virtual void Send(std::span<const char> span) = 0;
        virtual void Close() = 0;

        Message ReadNext()
        {
            auto message = std::move(m_received.front());
            m_received.pop_front();
            return message;
        }

        bool HasNext() const { return m_received.size(); };

    private:
        Messages m_received;
    };

}
