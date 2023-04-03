#pragma once

#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace psm
{
    class Log
    {
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_CoreLogger; }
    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
    };

#define LOG_TRACE(...)		::psm::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)		::psm::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)		::psm::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)		::psm::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)   ::psm::Log::GetLogger()->critical(__VA_ARGS__)
}