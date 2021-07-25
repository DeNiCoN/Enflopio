#pragma once



namespace Enflopio
{

    namespace ClientMessages
    {
        struct Hello;
    }

    namespace ServerMessages
    {
        struct Hello;
    }

    class ServerProtocol
    {
    private:
        virtual void Handle(const ServerMessages::Hello& msg) = 0;
    };

    class ClientProtocol
    {
    private:
        virtual void Handle(const ClientMessages::Hello& msg) = 0;
    };
}
