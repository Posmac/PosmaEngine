#pragma once

#include <string>
#include <stdexcept>

#include "RHI/Interface/Types.h"

#include "Include/vulkan/vulkan.h"
#include "stb_image.h"

namespace psm
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

    class TextureLoader
    {
        //singleton realization
    public:
        TextureLoader(TextureLoader&) = delete;
        void operator=(const TextureLoader&) = delete;
        static TextureLoader* Instance();
    private:
        TextureLoader() = default;
        static TextureLoader* s_Instance;
        //class specific
    public:
        void LoadRawTextureData(const std::string& path, RawTextureData* data);
        void CleanRawTextureData(void* data);
        void AddWhiteDefaultTexture(ImagePtr& whiteTexture);
        ImagePtr GetWhiteTexture() const;
        void Deinit();

    private:
        ImagePtr mWhiteTexture;
    };
}