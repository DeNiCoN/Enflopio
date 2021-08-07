#pragma once
#include "world.hpp"
#include "controls_state.hpp"
#include <GLFW/glfw3.h>
#include "network_manager.hpp"

namespace Enflopio
{
    class InputManager
    {
    public:
        ControlsState GetCurrent();
        void Update();
        void Init(GLFWwindow* window)
        {
            m_window = window;
        }

        void SendControls(NetworkManager& network);
    private:
        GLFWwindow* m_window;
    };
}
