#pragma once

#include "Common.h"

namespace psm
{
    void LogMessage(MessageSeverity severity, const std::string& message);
    void FatalError(const std::string& message);

    template<typename T>
    static void Assert(bool condition, const T& errorMsg)
    {
        if(!condition)
        {
            FatalError(errorMsg);
        }
    }
}