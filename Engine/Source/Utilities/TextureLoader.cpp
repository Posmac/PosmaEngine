#include "TextureLoader.h"

#include "RHI/Vulkan/CVkImage.h"

#include <iostream>
#include <fstream>

namespace psm
{

    TextureLoader* TextureLoader::s_Instance = nullptr;

    TextureLoader* TextureLoader::Instance()
    {
        if (s_Instance == nullptr)
        {
            s_Instance = new TextureLoader();
        }

        return s_Instance;
    }

    void TextureLoader::LoadRawTextureData(const std::string& path, RawTextureData* data)
    {
        data->Data = stbi_load(path.c_str(), &data->Width, &data->Height,
            &data->NrChannels, data->Type);

        if (!data->Data)
        {
            throw std::runtime_error("Failed to load texture with path:" + path);
        }
    }

    void TextureLoader::CleanRawTextureData(void* data)
    {
        stbi_image_free(data);
    }

    RawTextureData::RawTextureData(RGB_Type type)
    {
        Width = Height = NrChannels = 0;
        Data = nullptr;
        Type = type;
    }

    void TextureLoader::AddWhiteDefaultTexture(ImagePtr& whiteTexture)
    {
        mWhiteTexture = whiteTexture;
    }

    ImagePtr TextureLoader::GetWhiteTexture() const
    {
        return mWhiteTexture;
    }

    void TextureLoader::Deinit()
    {
        mWhiteTexture = nullptr;
    }
}