#include "TimeSystem.h"

#include <iostream>

namespace psm
{
    GlobalTimer* GlobalTimer::s_Instance = nullptr;;

    GlobalTimer* GlobalTimer::Instance()
    {
        if(s_Instance == nullptr)
        {
            s_Instance = new GlobalTimer();
        }

        return s_Instance;
    }

    void GlobalTimer::Init(int targetFrameRate)
    {
        m_TargetFrameRate = targetFrameRate;
        Reset();
    }

    void GlobalTimer::Reset()
    {
        m_FramesElapsed = 0;
        m_DeltaTime = 0;
        m_TotalTime = 0;
        m_PrevTime = std::chrono::high_resolution_clock::now();
    }

    bool GlobalTimer::IsTimeElapsed()
    {
        using namespace std::chrono;
        time_point currentTime = high_resolution_clock::now();

        milliseconds time = duration_cast<milliseconds>(currentTime - m_PrevTime);

        m_DeltaTime = time.count() / 1000.0f;
        m_TotalTime += m_DeltaTime;
        m_FramesElapsed++;

        m_PrevTime = currentTime;

        return true;
    }

    float GlobalTimer::DeltaTime() const
    {
        return m_DeltaTime;
    }

    float GlobalTimer::TotalTime() const
    {
        return m_TotalTime;
    }
}