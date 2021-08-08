#pragma once
#include <cstdint>
#include <chrono>
#include "connection.hpp"
#include "tcp_connection_listener.hpp"
#include "websocket_connection_listener.hpp"
#include "protocol_impl.hpp"
#include "world.hpp"

namespace Enflopio
{
    class Server
    {
    public:
        struct Options;
        explicit Server(const Options& options);

        void Run();

        struct Options
        {
            std::uint16_t tcp_port = 25325;
            std::uint16_t websocket_port = 25326;
        };

        void NewConnection(Connection::Ptr);
        void StopConnection(Connection::Ptr);
    private:
        void StartListening();
        void StopListening();
        void UpdateLoop();
        void ProcessMessages();

        void InitClock();
        double UpdateClock();

        void UpdateGame(double delta);
        bool ShouldClose() const { return false; }

        using TimePoint = std::chrono::high_resolution_clock::time_point;
        using Duration = std::chrono::high_resolution_clock::duration;
        TimePoint m_last_update;
        Duration m_lag;
        Duration m_frame = std::chrono::duration_cast<Duration>(std::chrono::duration<double>(1.0/60.0));
        TimePoint m_current_update;

        std::vector<std::pair<Connection::Ptr, ProtocolImpl>> m_connections;
        std::mutex m_connections_mutex;
        TCPConnectionListener m_tcp_listener;
        WebSocketConnectionListener m_websocket_listener;

        World m_world;
    };
}
