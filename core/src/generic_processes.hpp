#pragma once
#include "process_manager.hpp"
#include <variant>
#include <array>

namespace Enflopio
{
    namespace Processes
    {
        class None : public Process
        {
        public:
            void Update(double delta) { Stop(); }
        };

        class Wait : public Process
        {
        public:
            Wait(double time) : m_time(time)
            {}

            void Update(double delta)
            {
                m_time_passed += delta;
                if (m_time_passed > m_time)
                    Stop();
            }

        private:
            double m_time;
            double m_time_passed = 0.;
        };

        class Endless : public Process
        {
        public:
            void Update(double delta) {}
        };

        template<typename C>
        class Callback : public Process
        {
        public:
            Callback(C callback) : m_callback(callback)
            {}

            void Init() override
            {
                m_callback();
            }

            void Update(double delta) override
            {
                Stop();
            }

        private:
            C m_callback;
        };

        template<typename C>
        class WhileTrue : public Process
        {
        public:
            WhileTrue(C callback) : m_callback(callback)
            {}

            void Init() override
            {
            }

            void Update(double delta) override
            {
                m_callback(delta);
            }

        private:
            C m_callback;
        };

        template<typename C>
        class Generic : public Process
        {
        public:
            Generic(C callback) : m_callback(callback)
            {}

            void Init() override
            {
            }

            void Update(double delta) override
            {
                if (!m_callback(delta))
                {
                    Stop();
                }
            }

        private:
            C m_callback;
        };

        template<typename C, typename S>
        class Timed : public Process
        {
        public:
            Timed(double time, C callback, S stop_callback = []{}) :
                m_callback(callback), m_time(time), m_stop_callback(stop_callback)
            {}

            void Update(double delta)
            {
                if (m_time_passed >= m_time)
                {
                    m_stop_callback();
                    Stop();
                    return;
                }
                m_callback(delta);
                m_time_passed += delta;
            }

        private:
            C m_callback;
            double m_time;
            S m_stop_callback;
            double m_time_passed = 0.;
        };

        template<typename C>
        class InterpolateCallback : public Process
        {
        public:
            InterpolateCallback(double time, C callback) :
                m_time(time), m_callback(callback)
            {}

            void Update(double delta)
            {
                if (m_time_passed >= m_time)
                {
                    Stop();
                    return;
                }
                m_callback(m_time_passed / m_time);
                m_time_passed += delta;
            }

        private:
            C m_callback;
            double m_time;
            double m_time_passed = 0.;
        };

        template<typename T>
        class ProcessWrapper
        {
        public:
            ProcessWrapper(T&& t) : m_t(std::forward<T>(t))
            {}

            void Init() { m_t.Init(); }
            void Update(double delta) { m_t.Update(delta); }
            void SetNext(ProcessPtr n) { m_t.SetNext(std::move(n)); }
            const Process* GetNext() const { return m_t.GetNext(); }
            Process* GetNext() { return m_t.GetNext(); }
            ProcessPtr TakeNext() { return m_t.TakeNext(); }
            void Stop() { m_t.Stop(); }
            Process::State GetState() { return m_t.GetState(); }
        private:
            T m_t;
        };

        template<typename T>
        class ProcessWrapper<std::unique_ptr<T>>
        {
        public:
            ProcessWrapper(std::unique_ptr<T> t) : m_t(std::move(t))
            {}

            void Init() { m_t->Init(); }
            void Update(double delta) { m_t->Update(delta); }
            void SetNext(ProcessPtr n) { m_t->SetNext(std::move(n)); }
            const Process* GetNext() const { return m_t->GetNext(); }
            Process* GetNext() { return m_t->GetNext(); }
            ProcessPtr TakeNext() { return m_t->TakeNext(); }
            void Stop() { m_t->Stop(); }
            Process::State GetState() { return m_t->GetState(); }
        private:
            std::unique_ptr<T> m_t;
        };

        //TODO ignores child processes
        //TODO Use std::array instead of vector
        template<typename ...Args>
        class Consecutive : public Process
        {
        private:
            using Variant = std::variant<ProcessWrapper<Args>...>;
            using Container =
                std::vector<Variant>;
        public:
            Consecutive(ProcessWrapper<Args>&&... args)
                : Consecutive(std::make_index_sequence<sizeof...(Args)>{},
                              std::forward<ProcessWrapper<Args>>(args)...)
            {
            }

            void Init() override
            {
                std::visit([](auto& pr)
                {
                    pr.Init();
                }, *m_current);
            }

            void Update(double delta) override
            {
                if (std::visit([delta](auto& pr)
                    {
                        pr.Update(delta);
                        return pr.GetState() == Process::State::ENDED;
                    }, *m_current))
                {
                    ++m_current;
                    if (m_current == m_container.end())
                    {
                        Stop();
                        return;
                    }
                    std::visit([](auto& pr)
                    {
                        pr.Init();
                    }, *m_current);
                }
            }

        private:
            template<std::size_t ... Is>
            Consecutive(const std::index_sequence<Is...>&,
                        ProcessWrapper<Args>&&... args)
            {
                (... , m_container.emplace_back(Variant(std::in_place_index<Is>,
                                   std::forward<ProcessWrapper<Args>>(args))));
                m_current = m_container.begin();
            }

            Container m_container;
            typename Container::iterator m_current = m_container.begin();
        };

        template<typename ... Args>
        class And : public Process
        {
        public:
            And(ProcessWrapper<Args>&& ... args)
                : m_args(std::forward<ProcessWrapper<Args>>(args)...)
            {
            }

            void Init() override
            {
                std::apply([](auto && ... args)
                {
                    (..., args.Init());
                }, m_args);
            }

            void Update(double delta) override
            {
                if (!std::apply([delta](auto && ... args)
                {
                    return (... && (args.Update(delta),
                                    args.GetState() != Process::State::ENDED));
                }, m_args))
                {
                    Stop();
                }
            }

        private:
            std::tuple<ProcessWrapper<Args>...> m_args;
        };

        template<typename ... Args>
        Consecutive<Args...> MakeConsecutive(Args&& ... args)
        {
            return Consecutive<Args...>(ProcessWrapper(std::forward<Args>(args))...);
        }

        template<typename ... Args>
        And<Args...> MakeAnd(Args&& ... args)
        {
            return And<Args...>(ProcessWrapper(std::forward<Args>(args))...);
        }

        template<typename P>
        ProcessPtr ToPtr(P&& p)
        {
            return std::make_unique<P>(std::forward<P>(p));
        }
    }
}
