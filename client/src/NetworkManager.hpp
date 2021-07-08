#pragma once
#include <string>
#include <deque>

namespace Enflopio
{
    class NetworkManager
    {
    public:
        using Message = std::string;
        using Messages = std::deque<Message>;

        void Init();

        Message NextMessage();
        Message HasNextMessage();

        void Send(const Message& message);
    };
}
