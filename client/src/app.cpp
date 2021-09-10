#include "app.hpp"
#include <glm/glm.hpp>
#include "gl_includes.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "circle_batch.hpp"
#include "messages.hpp"
#include <signal.h>

#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

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

    void App::SetupLogging()
    {
        auto frame = spdlog::stdout_color_mt("frame");
        auto network = spdlog::stdout_color_mt("network");

        frame->enable_backtrace(NUM_BACKTRACE_LOG_MESSAGES);
        frame->set_level(spdlog::level::warn);

        network->enable_backtrace(NUM_BACKTRACE_LOG_MESSAGES);
        network->set_level(spdlog::level::warn);

        std::set_terminate([]
        {
            spdlog::get("frame")->dump_backtrace();
            spdlog::get("network")->dump_backtrace();
            std::abort();
        });

        logging::frame = frame;
        logging::net = network;
    }

    void App::Init()
    {
        SetupLogging();

        spdlog::info("Logging set");

        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
            exit(EXIT_FAILURE);
        spdlog::info("Glfw initialized");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        m_window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);
        if (!m_window)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        spdlog::info("Window created");
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

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = NULL;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init();

        m_network.Init();
        spdlog::info("Network initialized");
        m_network.Send(Serialize(ServerMessages::Hello()));
        spdlog::info("Initialization finished");
    }

    void App::PreUpdate()
    {
        logging::frame->info("Frame {} stared", m_tick);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        logging::frame->info("Processing network messages");
        while (m_network.HasNextMessage())
        {
            auto message_ptr = ClientMessages::Deserialize(m_network.NextMessage());
            message_ptr->Accept(m_protocol);
        }
    }

    void App::Simulate(double delta)
    {
        logging::frame->info("Simulation {}", GetSimulationTick());

        logging::frame->info("Updating controls");
        m_input_manager.Update();
        auto current_controls = m_input_manager.GetCurrent();

        if (m_world.HasPlayer(m_current_player_id))
        {
            m_world.GetPlayer(m_current_player_id).SetControls(current_controls);

            //If controls changed, send them to server
            m_network_input.ProcessControls(current_controls, delta);
        }

        logging::frame->info("Interpolating");
        for (auto& [id, interp] : m_interpolations)
        {
            interp.Update(delta);
        }

        logging::frame->info("Running world update");
        m_process_manager.Update(delta);
        m_world.Update(delta);
        m_camera.Update(delta);

        if (m_world.HasPlayer(m_current_player_id))
        {
            logging::frame->info("Moving camera");
            m_camera.Move(m_world.GetPlayer(m_current_player_id).position);
        }
    }

    void App::PostUpdate()
    {
        logging::frame->info("Frame {} finished", GetFrameTick());
    }

    void App::PostSimulate(double delta)
    {
        logging::frame->info("Render update", m_tick);
        CircleBatch::Instance().Clear();

        //Add to Render
        for (const auto& circle : m_world.GetCircles())
            CircleBatch::Instance().Add(circle);

        for (const auto& [id, player] : m_world.GetPlayers())
            CircleBatch::Instance().Add(player);

        for (const auto& [pos, color] : m_debug_circles)
        {
            World::Circle cir;
            cir.position = pos;
            cir.radius = 0.5;
            CircleBatch::Instance().Add(cir, color);
        }
    }

    bool App::ShouldStop()
    {
        return glfwWindowShouldClose(m_window);
    }

    void App::Render(double delay)
    {
        logging::frame->info("Render", m_tick);
        glClear(GL_COLOR_BUFFER_BIT);

        CircleBatch::Instance().SetView(m_camera.CalculateView(delay));
        CircleBatch::Instance().Draw(delay);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
    }

    void App::Terminate()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void App::AddDebugDraw(glm::vec2 position, glm::vec4 color)
    {
        m_debug_circles.push_back({position, color});
    }
}
