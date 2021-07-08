#pragma once
#include "Protocol.hpp"

namespace Enflopio
{
    template <typename Protocol, typename T>
    class MessageBase
    {
    public:
        void Visit();
    };

    namespace ClientMessages
    {
        template <typename T>
        using Base = MessageBase<ClientProtocol, T>;

        struct Hello : public Base<Hello>
        {

        };
    }

    namespace ServerMessages
    {
        template <typename T>
        using Base = MessageBase<ServerProtocol, T>;

        struct Hello : public Base<Hello>
        {

        };

    }
}
