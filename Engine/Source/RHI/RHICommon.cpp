#include "RHICommon.h"

#include <iostream>
#include <format>
#include <Windows.h>

namespace psm
{
    static SeverityMessageCallback CallbackFn = [](MessageSeverity severity, const std::string& message)
    {
        constexpr auto severityToStr = [](MessageSeverity severity)
        {
            switch(severity)
            {
                case psm::MessageSeverity::Info:
                    return "Info";
                case psm::MessageSeverity::Warning:
                    return "Warning";
                case psm::MessageSeverity::Error:
                    return "Error";
                case psm::MessageSeverity::Fatal:
                    return "Fatal";
            }
        };

        std::cout << "RHI [" << severityToStr(severity) << "]: " << message << '\n';
#ifdef _WIN32
        std::string str = std::format("RGL [{}] - {}\n", severityToStr(severity), message);
        OutputDebugStringA(str.c_str());
#endif
    };

    void LogMessage(MessageSeverity severity, const std::string& message)
    {
        CallbackFn(severity, message);
        if(severity == MessageSeverity::Fatal)
        {
            throw std::runtime_error(message);
        }
    }

    void FatalError(const std::string& message)
    {
        LogMessage(MessageSeverity::Fatal, message);
    }
}
