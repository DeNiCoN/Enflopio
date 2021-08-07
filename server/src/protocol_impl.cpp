#include "protocol_impl.hpp"
#include <spdlog/spdlog.h>
#include "messages.hpp"

namespace Enflopio
{
    void ProtocolImpl::Handle(const ServerMessages::Hello &msg)
    {
        spdlog::debug("Hello received");
        m_connection.Send(Serialize(ClientMessages::Hello()));
    }

    void ProtocolImpl::Handle(const ServerMessages::Input &msg)
    {
        spdlog::debug("Input received");
    }
}
