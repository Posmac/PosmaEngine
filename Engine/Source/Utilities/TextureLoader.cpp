#include "TextureLoader.h"

#include <iostream>
#include <fstream>

namespace psm
{
    namespace putils
    {
        void LoadRawTextureData(const std::string& path, RawTextureData* data)
        {
            data->Data = stbi_load(path.c_str(), &data->Width, &data->Height,
                &data->NrChannels, data->Type);

            if (!data->Data)
            {
                throw std::runtime_error("Failed to load texture with path:" + path);
            }
        }

        void CleanRawTextureData(void* data)
        {
            stbi_image_free(data);
        }

        RawTextureData::RawTextureData(RGB_Type type)
        {
            Width = Height = NrChannels = 0;
            Data = nullptr;
            Type = type;
        }
    }
}