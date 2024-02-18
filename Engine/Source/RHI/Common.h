#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace psm
{
    enum class MessageSeverity : uint8_t
    {
        Info = 0,
        Warning,
        Error,
        Fatal
    };

    using SeverityMessageCallback = std::function<void>(MessageSeverity, const std::string& message);

    enum class API
    {
        Unitialized,
        Default,
        Vulkan,
        Direct3D12,
        WebGPU,
        Metal,
    };

    struct InitOptions
    {
        API CurrentApi;
        SeverityMessageCallback DebugInfoCallback;
        std::string AppName;
        std::string EngineName;
        struct Version
        {
            uint8_t variant = 0;
            uint8_t major = 0;
            uint8_t minor = 0;
            uint8_t patch = 0;
        } AppVersion, EngineVersion;
    };
}