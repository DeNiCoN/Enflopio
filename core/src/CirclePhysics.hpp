#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <spdlog/spdlog.h>
//#include <ranges>

namespace Enflopio
{
    class CirclePhysics
    {
    public:
        struct Circle
        {
            glm::vec2 position = {0, 0};
            glm::vec2 velocity = {0, 0};
            glm::vec2 acceleration = {0, 0};
            float radius = 1;
        };
        using Circles = std::vector<Circle>;

        void Proccess(Circles& dynamic_circles, const std::vector<Circle*>& static_circles, double delta)
        {
            for (auto& circle : dynamic_circles)
            {
                glm::vec2 v_half = circle.velocity + 0.5f*circle.acceleration*static_cast<float>(delta);

                circle.position += v_half*static_cast<float>(delta);

                glm::vec2 accel = {0, 0};
                for (auto* s : static_circles)
                {
                    auto dif = s->position - circle.position;
                    accel += 50.f * glm::normalize(dif) * (1.f/glm::length(dif));
                }
                circle.acceleration = accel;
                circle.velocity = v_half + 0.5f*circle.acceleration*static_cast<float>(delta);
            }

            ResolveCollisions(dynamic_circles, static_circles, delta);

        }

        void ResolveCollisions(Circles& dynamic_circles, const std::vector<Circle*>& static_circles, double delta)
        {
//            for (int i = 0; i < dynamic_circles.size(); i++)
//            {
//
//            }
            for (auto& circle1 : dynamic_circles)
            {
                for (auto& circle2 : dynamic_circles)
                {
                    if (circle1.position == circle2.position)
                        continue;
                    auto dif = circle1.position - circle2.position;
                    float length = glm::length(dif);
                    if (length < (circle1.radius + circle2.radius) / 2.0f)
                    {
                        //spdlog::debug("Collision {},{} {},{}", circle1.position.x, circle1.position.y, circle2.position.x, circle2.position.y);
                        circle1.position += glm::normalize(dif)*((circle1.radius + circle2.radius) / 2.0f - length);
                        circle1.velocity = {0, 0};
                    }
                }

                for (auto& circle2 : static_circles)
                {
                    auto dif = circle1.position - circle2->position;
                    float length = glm::length(dif);
                    if (length < (circle1.radius + circle2->radius)/2.f)
                    {
                        circle1.position += glm::normalize(dif)*((circle1.radius + circle2->radius)/2.0f - length);
                        circle1.velocity = {0, 0};
                    }
                }
            }
        }
    };
}
