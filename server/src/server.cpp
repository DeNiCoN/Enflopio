#include "server.hpp"
#include <chrono>
#include <span>

using namespace std;

namespace Enflopio
{
    Server::Server(const Options& options)
        : m_tcp_listener(options.tcp_port, *this)
    {

    }

    void Server::Run()
    {
        StartListening();
        InitClock();

        while (!ShouldClose())
        {
            UpdateLoop();
        }

        StopListening();
    }

    void Server::StartListening()
    {
        m_tcp_listener.StartListening();
    }

    void Server::StopListening()
    {
        m_tcp_listener.StopListening();
    }

    void Server::InitClock()
    {
        m_current_update = chrono::high_resolution_clock::now();
    }

    void Server::UpdateLoop()
    {
        const auto delta = UpdateClock();
        ProcessMessages();
        UpdateGame(delta);
    }

    void Server::ProcessMessages()
    {

    }

    void Server::UpdateGame(double delta)
    {

    }

    double Server::UpdateClock()
    {
        m_last_update = m_current_update;
        m_current_update = chrono::high_resolution_clock::now();
        return chrono::duration<double>(m_current_update - m_last_update).count();
    }

    void Server::NewConnection(Connection::Ptr connection)
    {
        static char buf[6] = "Hello";
        std::lock_guard lock(m_connections_mutex);
        buf[4]++;
        connection->Send(buf);
        connection->Close();
        m_connections.push_back(std::move(connection));
    }
}
