#include "App.hpp"
#include <glm/glm.hpp>
#include "GlIncludes.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "CircleBatch.hpp"

namespace Enflopio
{
    static void error_callback(int error, const char *description)
    {
        spdlog::error("Error: {}\n", description);
    }

    //TODO Do something about this
    App* app_instance;
    void window_size_callback(GLFWwindow* window, int width, int height)
    {
        app_instance->m_camera.SetBorders(width, height);
        app_instance->m_camera.SetScale(width * 0.05);
        CircleBatch::Instance().SetProjection(app_instance->m_camera.GetProjection());
        glViewport(0, 0, width, height);
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    void App::Init()
    {
        app_instance = this;

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

        //NetworkManager::Init();

        m_current_player = std::make_shared<World::Player>();
        m_current_player->position = {0, 0};
        m_current_player->velocity = {0, 0};
        m_world.AddPlayer(m_current_player);

        for (int i = 0; i < 100; i++)
        {
            m_world.AddCircle({{rand() % 100 - 50, rand() % 100 - 50}});

        }

        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        m_camera.SetBorders(width, height);
        CircleBatch::Instance().SetProjection(app_instance->m_camera.GetProjection());

        // One unit is 5% of width: (1*scale) / width = 0.05;
        m_camera.SetScale(width * 0.05);
        m_camera.SetPosition(m_current_player->position);
        glViewport(0, 0, width, height);
        glClearColor(0, 0, 0, 1);
    }

    void App::Update(double delta)
    {
        CircleBatch::Instance().Clear();

        m_input_manager.Update();
        auto current_controls = m_input_manager.GetCurrent();
        m_current_player->SetControls(current_controls);

        m_world.Update(delta);

        m_camera.Move(m_current_player->position);

        m_camera.Update(delta);

        for (const auto& circle : m_world.GetCircles())
            CircleBatch::Instance().Add(circle);

        for (const auto& player : m_world.GetPlayers())
            CircleBatch::Instance().Add(*player);
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
