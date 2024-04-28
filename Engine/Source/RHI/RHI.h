#pragma once

#include "Common.h"

namespace psm
{
    API CurrentAPI();
    void InitRHI(const InitOptions& options);
    void ShutdownRHI();
}