#pragma once
#include "protocol.hpp"
#include "player.hpp"

namespace Enflopio
{
    class App;

    class ProtocolImpl final : public ClientProtocol
    {
    public:
        ProtocolImpl(App& app) : m_app(app)
        {

        }

        void Handle(const ClientMessages::Hello& msg) override;
        void Handle(const ClientMessages::Sync& msg) override;
    private:
        App& m_app;
    };
}
