#include "app.hpp"
#include <glm/glm.hpp>
#include "gl_includes.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "circle_batch.hpp"
#include "messages.hpp"

namespace Enflopio
{
    App& App::Instance()
    {
        static App instance;
        return instance;
    }

    static void error_callback(int error, const char *description)
    {
        spdlog::error("Error: {}\n", description);
    }

    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        App::Instance().Resize(width, height);
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void App::Resize(int width, int height)
    {
        m_camera.SetBorders(width, height);
        // One unit is 5% of width: (1*scale) / width = 0.05;
        m_camera.SetScale(width * 0.05);
        CircleBatch::Instance().SetProjection(m_camera.GetProjection());
        glViewport(0, 0, width, height);
    }

    void App::Init()
    {
        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
            exit(EXIT_FAILURE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        m_window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);
        if (!m_window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwSetWindowSizeCallback(m_window, window_size_callback);

        m_input_manager.Init(m_window);

        glfwMakeContextCurrent(m_window);

#ifdef __EMSCRIPTEN__
#else
        gladLoadGL();
#endif
        glfwSwapInterval(1);
        glClearColor(0, 0, 0, 1);

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        Resize(width, height);

        m_network.Init();
        m_network.Send(Serialize(ServerMessages::Hello()));
    }

    void App::PreUpdate()
    {

    }

    void App::Update(double delta)
    {
        CircleBatch::Instance().Clear();

        //process new messages
        while (m_network.HasNextMessage())
        {
            auto message_ptr = ClientMessages::Deserialize(m_network.NextMessage());
            message_ptr->Accept(m_protocol);
        }

        //Update controls
        m_input_manager.Update();
        auto current_controls = m_input_manager.GetCurrent();

        if (m_world.HasPlayer(m_current_player_id))
        {
            //Client side prediction. Kind of, it's not done yet
            m_world.GetPlayer(m_current_player_id).SetControls(current_controls);

            //If controls changed, send them to server
            m_network_input.ProcessControls(current_controls);
        }

        //Update simulation
        m_world.Update(delta);
        m_camera.Update(delta);
        if (m_world.HasPlayer(m_current_player_id))
            m_camera.Move(m_world.GetPlayer(m_current_player_id).position);

        //Add to Render
        for (const auto& circle : m_world.GetCircles())
            CircleBatch::Instance().Add(circle);

        for (const auto& [id, player] : m_world.GetPlayers())
            CircleBatch::Instance().Add(player);

        for (const auto& pos : m_debug_circles)
        {
            World::Circle cir;
            cir.position = pos;
            cir.radius = 0.5;
            CircleBatch::Instance().Add(cir);
        }
    }

    bool App::ShouldStop()
    {
        return glfwWindowShouldClose(m_window);
    }

    void App::Render(double delay)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        CircleBatch::Instance().SetView(m_camera.CalculateView(delay));
        CircleBatch::Instance().Draw(delay);

        glfwSwapBuffers(m_window);
    }

    void App::Terminate()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}
