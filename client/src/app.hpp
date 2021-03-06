#pragma once
#include "game_loop.hpp"
#include "circle_batch.hpp"
#include "world.hpp"
#include <GLFW/glfw3.h>
#include "input_manager.hpp"
#include "camera.hpp"
#include "network_manager.hpp"
#include "protocol_impl.hpp"
#include "network_input_manager.hpp"
#include "process_manager.hpp"
#include "interpolation_process.hpp"
#include <unordered_map>

namespace Enflopio
{
    class App final : public GameLoop<App>
    {
    public:
        void AddDebugDraw(glm::vec2 position, glm::vec4 color);
        void ClearDebugDraw()
        {
            m_debug_circles.clear();
        }


        ProcessManager& GetProcessManager() { return m_process_manager; }
        const ProcessManager& GetProcessManager() const { return m_process_manager; }

        void SetupLogging();
        void Init();
        void PreUpdate();
        void PostUpdate();
        void Simulate(double delta);
        void PostSimulate(double delta);
        void Render(double delay);
        bool ShouldStop();
        void Terminate();
    private:
        static constexpr std::size_t NUM_BACKTRACE_LOG_MESSAGES = 128;

        App() : m_protocol(*this), m_network_input(m_network, m_process_manager) {};
        World m_world;
        ProtocolImpl m_protocol;
        NetworkManager m_network;
        World::PlayerID m_current_player_id;
        GLFWwindow* m_window;
        InputManager m_input_manager;
        ProcessManager m_process_manager;
        NetworkInputManager m_network_input;
        Camera m_camera;

        std::uint64_t m_tick = 0;

        std::unordered_map<World::PlayerID, InterpolationProcess> m_interpolations;
        std::vector<std::pair<glm::vec2, glm::vec4>> m_debug_circles;

        void Resize(int width, int heigth);

        friend class ProtocolImpl;
        friend void window_size_callback(GLFWwindow*, int, int);
        friend class GameLoop<App>;
    };
}
