#pragma once

namespace psm
{
    union PlatformConfig
    {
        struct
        {
            void* hInstance;
            void* hWnd;
        } win32;
    };

    union DeviceData
    {
        struct
        {
            void* Device;
        } d3d12Data;

        struct
        {
            void* Device;
            void* Instance;
            void* PhysicalDevice;
            //uint32_t QueueFamilyIndex;
            uint32_t GraphicsQueueIndex;
            void* GraphicsQueue;
        } vkData;

        struct
        {
            void* Device;
        } mtlData;
    };
}