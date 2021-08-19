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

namespace Enflopio
{
    class App final : public GameLoop
    {
    public:
        static App& Instance();

        void AddDebugDraw(glm::vec2 position, glm::vec4 color);
        void ClearDebugDraw()
        {
            m_debug_circles.clear();
        }


        ProcessManager& GetProcessManager() { return m_process_manager; }
        const ProcessManager& GetProcessManager() const { return m_process_manager; }
    protected:
        void Init() final override;
        void PreUpdate() final override;
        void Update(double delta) final override;
        void Render(double delay) final override;
        bool ShouldStop() final override;
        void Terminate() final override;
    private:
        App() : m_protocol(*this), m_network_input(m_network) {};
        World m_world;
        ProtocolImpl m_protocol;
        NetworkManager m_network;
        World::PlayerID m_current_player_id;
        GLFWwindow* m_window;
        InputManager m_input_manager;
        NetworkInputManager m_network_input;
        Camera m_camera;
        ProcessManager m_process_manager;

        std::vector<std::pair<glm::vec2, glm::vec4>> m_debug_circles;

        void Resize(int width, int heigth);

        friend class ProtocolImpl;
        friend void window_size_callback(GLFWwindow*, int, int);
    };
}
