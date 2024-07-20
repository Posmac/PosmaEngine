#pragma once

#include <string>
#include <stdexcept>
#include <memory>
#include <unordered_map>

#include "RHI/RHICommon.h"
#include "RHI/Interface/Types.h"

#include "RHI/Enums/ImageFormats.h"

#include "stb_image.h"

namespace psm
{
    class TextureLoader
    {
    private:
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
            RawTextureData();
            explicit RawTextureData(const std::string& path, RGB_Type type);
            ~RawTextureData();

            int Width;
            int Height;
            int NrChannels;
            RGB_Type Type;
            stbi_uc* Data;
        };

        struct RawDdsData
        {
            int Width;
            int Height;
            EFormat Format;
            void* Mip0Data;
            void* Mip1Data;
            int BlockSize;
            int MipsCount;
            bool IsCubemap;
            bool IsCompressed;
            int RowPitch;
            int BitsPerPixel;
            int Depth;
            int PitchOrLinearSize;
        };

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
        void Init(DevicePtr device, CommandPoolPtr mCommandPool);
        ImagePtr LoadTexture(const std::string& path);
        ImagePtr GetWhiteTexture() const;
        void AddWhiteDefaultTexture(const std::string& path);
        void Deinit();
        ImagePtr LoadDDSTexture(const std::string& path);
    private:
        DevicePtr mDeviceInternal;
        CommandPoolPtr mCommandPoolInternal;

        ImagePtr mWhiteTexture;
        std::unordered_map<std::string, ImagePtr> mImages;
    };
}