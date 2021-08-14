#pragma once
#include "protocol.hpp"
#include "world.hpp"
#include "controls_state.hpp"

#include <sstream>
#include "serialization.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/unordered_map.hpp>

namespace Enflopio
{
    template <typename Protocol>
    class Message
    {
    public:
        using ID = std::uint32_t;
        virtual ~Message() = default;

        using Ptr = std::unique_ptr<Message>;

        virtual void Accept(Protocol& protocol) const = 0;

        virtual ID id() const = 0;
    };

    template <typename Protocol, typename T>
    class MessageBase : public Message<Protocol>
    {
    public:
        void Accept(Protocol& protocol) const override
        {
            protocol.Handle(*static_cast<const T*>(this));
        }
    };

    using ClientMessage = Message<ClientProtocol>;
    namespace ClientMessages
    {
        enum class Id : ClientMessage::ID
        {
            HELO,
            SYNC
        };

        template <typename T>
        using Base = MessageBase<ClientProtocol, T>;

        struct Hello : public Base<Hello>
        {
            Message::ID id() const override { return static_cast<Message::ID>(Id::HELO); }

            glm::vec2 player_pos;

            template <typename Archive>
            void serialize(Archive& ar)
            {
                ar(player_pos);
            }
        };

        struct Sync : public Base<Sync>
        {
            Message::ID id() const override { return static_cast<Message::ID>(Id::SYNC); }

            World::Players players;

            template <typename Archive>
            void serialize(Archive& ar)
            {
                ar(players);
            }
        };

        //TODO Consider adding generic for each visitor
        class SerializeVisitor : public ClientProtocol
        {
        public:
            SerializeVisitor() : ar(ss)
            {

            }

            void Handle(const Hello& msg)
            {
                Serialize(msg);
            }

            void Handle(const Sync& msg)
            {
                Serialize(msg);
            }

            std::string Get() const { return ss.str(); }

        private:
            template <typename T>
            void Serialize(const T& msg)
            {
                ss.clear();
                ar(msg.id(), msg);
            }

            std::ostringstream ss;
            cereal::PortableBinaryOutputArchive ar;
        };

        inline std::string Serialize(const ClientMessage& message)
        {
            SerializeVisitor visitor;
            message.Accept(visitor);
            //FIXME copy
            return visitor.Get();
        }

        inline ClientMessage::Ptr Deserialize(std::string message)
        {
            std::istringstream ss {std::move(message)};
            cereal::PortableBinaryInputArchive ar(ss);

            auto deserialize = [&ar]<typename T>(std::unique_ptr<T> result)
            {
                ar >> *result;
                return result;
            };

            ClientMessage::ID id;
            ar >> id;
            //FIXME checking
            switch (static_cast<Id>(id)) {
            case Id::HELO: return deserialize(std::make_unique<Hello>());
            case Id::SYNC: return deserialize(std::make_unique<Sync>());
            }

            return nullptr;
        }
    }

    using ServerMessage = Message<ServerProtocol>;
    namespace ServerMessages
    {
        enum class Id
        {
            HELO,
            INPT
        };
        template <typename T>
        using Base = MessageBase<ServerProtocol, T>;

        struct Hello : public Base<Hello>
        {
            Message::ID id() const override { return static_cast<Message::ID>(Id::HELO); }

            template <typename Archive>
            void serialize(Archive& ar)
            {
            }
        };

        struct Input : public Base<Input>
        {
            Message::ID id() const override { return static_cast<Message::ID>(Id::INPT); }
            ControlsState input;

            template <typename Archive>
            void serialize(Archive& ar)
            {
                ar(input);
            }
        };

        //TODO Consider adding generic for each visitor
        class SerializeVisitor : public ServerProtocol
        {
        public:
            SerializeVisitor() : ar(ss)
            {

            }

            void Handle(const Hello& msg)
            {
                Serialize(msg);
            }

            void Handle(const Input& msg)
            {
                Serialize(msg);
            }

            std::string Get() const { return ss.str(); }

        private:
            template <typename T>
            void Serialize(const T& msg)
            {
                ss.clear();
                ar(msg.id(), msg);
            }

            std::ostringstream ss;
            cereal::PortableBinaryOutputArchive ar;
        };

        inline ServerMessage::Ptr Deserialize(std::string message)
        {
            std::istringstream ss {std::move(message)};
            cereal::PortableBinaryInputArchive ar(ss);

            auto deserialize = [&ar]<typename T>(std::unique_ptr<T> result)
            {
                ar >> *result;
                return result;
            };

            ServerMessage::ID id;
            ar >> id;
            //FIXME checking
            switch (static_cast<Id>(id)) {
            case Id::HELO: return deserialize(std::make_unique<Hello>());
            case Id::INPT: return deserialize(std::make_unique<Input>());
            }

            return nullptr;
        }

        inline std::string Serialize(const ServerMessage& message)
        {
            SerializeVisitor visitor;
            message.Accept(visitor);
            //FIXME copy
            return visitor.Get();
        }
    }
}
