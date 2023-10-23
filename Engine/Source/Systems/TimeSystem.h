#pragma once

#include <chrono>

namespace psm
{
    class GlobalTimer
    {
        //singleton realization
    public:
        GlobalTimer(GlobalTimer&) = delete;
        void operator=(const GlobalTimer&) = delete;
        static GlobalTimer* Instance();
    private:
        GlobalTimer() = default;
        static GlobalTimer* s_Instance;
        //class specific
    public:
        void Init(int targetFrameRate);
        void Reset();
        bool IsTimeElapsed();
        float DeltaTime() const;
        float TotalTime() const;
    private:
        float m_TotalTime;
        float m_DeltaTime;
        std::chrono::steady_clock::time_point m_PrevTime;
        int m_TargetFrameRate;
        uint64_t m_FramesElapsed;
    };
}