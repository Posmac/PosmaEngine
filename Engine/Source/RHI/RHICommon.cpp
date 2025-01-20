#include "RHICommon.h"

#include <iostream>
#include <format>
#include <Windows.h>

namespace psm
{
    void LogMessage(MessageSeverity severity, const std::string& message)
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

        std::cout << "PSM [" << severityToStr(severity) << "]: " << message << '\n';
#ifdef _WIN32
        std::string str = std::format("PSM [{}] - {}\n", severityToStr(severity), message);
        OutputDebugStringA(str.c_str());
#endif

        if(severity >= MessageSeverity::Error)
        {
            //DebugBreak();
        }
    }

    void FatalError(const std::string& message)
    {
        LogMessage(MessageSeverity::Fatal, message);
    }
}