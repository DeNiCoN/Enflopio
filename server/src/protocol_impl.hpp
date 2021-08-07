#pragma once
#include "connection.hpp"
#include "protocol.hpp"

namespace Enflopio
{
    class ProtocolImpl final : public ServerProtocol
    {
    public:
        ProtocolImpl(Connection& connection)
            : m_connection(connection)
        {

        }

        void Handle(const ServerMessages::Hello& msg) override;
        void Handle(const ServerMessages::Input& msg) override;
    private:
        Connection& m_connection;
    };
}
