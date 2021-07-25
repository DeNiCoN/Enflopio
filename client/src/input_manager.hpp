#pragma once
#include "World.hpp"
#include "ControlsState.hpp"
#include <GLFW/glfw3.h>
#include "NetworkManager.hpp"

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
