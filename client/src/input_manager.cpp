#include "InputManager.hpp"

namespace Enflopio
{
    ControlsState InputManager::GetCurrent()
    {
        glm::vec2 result{0};
        int w = glfwGetKey(m_window, GLFW_KEY_W);
        int a = glfwGetKey(m_window, GLFW_KEY_A);
        int s = glfwGetKey(m_window, GLFW_KEY_S);
        int d = glfwGetKey(m_window, GLFW_KEY_D);
        if (w == GLFW_PRESS || w == GLFW_REPEAT)
            result += glm::vec2(0, 1);
        if (a == GLFW_PRESS || a == GLFW_REPEAT)
            result += glm::vec2(-1, 0);
        if (s == GLFW_PRESS || s == GLFW_REPEAT)
            result += glm::vec2(0, -1);
        if (d == GLFW_PRESS || d == GLFW_REPEAT)
            result += glm::vec2(1, 0);

        glm::normalize(result);
        return ControlsState{result};
    }

    void InputManager::Update()
    {
        glfwPollEvents();
    }
}
