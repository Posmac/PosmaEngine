#pragma once

#include <string>
#include <stdexcept>

#include "stb_image.h"

namespace psm
{
    namespace putils
    {
        enum RGB_Type
        {
            Default = 0, // only used for desired_channels
            Grey = 1,
            Grey_alpha = 2,
            Rgb = 3,
            Rgb_alpha = 4
        };

        struct RawTextureData
        {
            explicit RawTextureData(RGB_Type type);

            int Width;
            int Height;
            int NrChannels;
            RGB_Type Type;
            stbi_uc* Data;
        };

        void LoadRawTextureData(const std::string& path, RawTextureData* data);
        void CleanRawTextureData(void* data);
    }
}