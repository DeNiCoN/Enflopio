#pragma once
#include "Protocol.hpp"
#include "World.hpp"
#include "ControlsState.hpp"

namespace Enflopio
{
    template <typename Protocol>
    class Message
    {
    public:
        using Ptr = std::unique_ptr<Message>;

        virtual void Visit(const Protocol& protocol) = 0;
    };

    template <typename Protocol, typename T>
    class MessageBase : public Message<Protocol>
    {
    public:
        void Visit(const Protocol& protocol) override
        {
            protocol.handle(*static_cast<T*>(this));
        }
    };

    using ClientMessage = Message<ClientProtocol>;
    namespace ClientMessages
    {
        template <typename T>
        using Base = MessageBase<ClientProtocol, T>;

        struct Hello : public Base<Hello>
        {

        };

        ClientMessage::Ptr Deserialize(std::string_view message);
        std::string Serialize(const ClientMessage& message);
    }

    using ServerMessage = Message<ServerProtocol>;
    namespace ServerMessages
    {
        template <typename T>
        using Base = MessageBase<ServerProtocol, T>;

        struct Hello : public Base<Hello>
        {

        };

        struct Input : public Base<Input>
        {
            World::PlayerID id;
            ControlsState input;
        };

        ServerMessage::Ptr Deserialize(std::string_view message);
    }
}
