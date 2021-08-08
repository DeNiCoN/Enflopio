#pragma once

namespace Enflopio
{
    template <typename Protocol, typename T>
    class MessageBase;

    namespace ClientMessages
    {
        struct Hello;
        struct Sync;
    }

    namespace ServerMessages
    {
        struct Hello;
        struct Input;
    }

    class ClientProtocol
    {
    public:
        virtual ~ClientProtocol() = default;

    private:
        virtual void Handle(const ClientMessages::Hello& msg) = 0;
        virtual void Handle(const ClientMessages::Sync& msg) = 0;

        template <typename Protocol, typename T>
        friend class MessageBase;
    };

    class ServerProtocol
    {
    public:
        virtual ~ServerProtocol() = default;

    private:
        virtual void Handle(const ServerMessages::Hello& msg) = 0;
        virtual void Handle(const ServerMessages::Input& msg) = 0;

        template <typename Protocol, typename T>
        friend class MessageBase;
    };
}
