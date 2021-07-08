#pragma once
#include "GameLoop.hpp"
#include "CircleBatch.hpp"
#include "World.hpp"
#include <GLFW/glfw3.h>
#include "InputManager.hpp"
#include "Camera.hpp"
#include "NetworkManager.hpp"

namespace Enflopio
{
    class App : public GameLoop
    {
    public:
    protected:
        void Init() final override;
        void Update(double delta) final override;
        void Render(double delay) final override;
        bool ShouldStop() final override;
        void Terminate() final override;
    private:
        World m_world;
        //NetworkManager m_network;
        World::PlayerPtr m_current_player;

        GLFWwindow* m_window;
        InputManager m_input_manager;
        Camera m_camera;

        friend void window_size_callback(GLFWwindow*, int, int);
    };
}
