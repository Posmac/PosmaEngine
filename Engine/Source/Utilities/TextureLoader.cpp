#include "TextureLoader.h"

#if defined(RHI_VULKAN)
#include "RHI/Vulkan/CVkImage.h"
#include "RHI/Vulkan/CVkDevice.h"
#endif

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

    TextureLoader::RawTextureData::RawTextureData(const std::string& path, RGB_Type type)
    {
        Width = Height = NrChannels = 0;
        Data = nullptr;
        Type = type;

        Data = stbi_load(path.c_str(), &Width, &Height,
                              &NrChannels, Type);

        if(Data == nullptr)
        {
            LogMessage(MessageSeverity::Error, "Failed to load texture with path:");
        }
    }

    TextureLoader::RawTextureData::~RawTextureData()
    {
        stbi_image_free(Data);
    }

    void TextureLoader::AddWhiteDefaultTexture(const std::string& path)
    {
        mWhiteTexture = LoadTexture(path);
    }

    ImagePtr TextureLoader::GetWhiteTexture() const
    {
        return mWhiteTexture;
    }

    void TextureLoader::Init(DevicePtr device, CommandPoolPtr mCommandPool)
    {
        mDeviceInternal = device;
        mCommandPoolInternal = mCommandPool;
    }

    ImagePtr TextureLoader::LoadTexture(const std::string& path)
    {
        if(mImages.count(path) != 0)
        {
            return mImages[path];
        }

        RawTextureData textureData(path, Rgb_alpha);

        if(textureData.Data == nullptr)
        {
            LogMessage(MessageSeverity::Error, "Raw texture data pointer is null");
        }

        SImageConfig imageConfig =
        {
            .ImageSize = { (uint32_t)textureData.Width, (uint32_t)textureData.Height, 1 },
            .MipLevels = static_cast<int>(1), //implement later
            .ArrayLevels = 1,
            .Type = EImageType::TYPE_2D,
            .Format = EFormat::R8G8B8A8_SRGB,
            .Tiling = EImageTiling::OPTIMAL,
            .InitialLayout = EImageLayout::UNDEFINED,
            .Usage = EImageUsageType::USAGE_TRANSFER_SRC_BIT | EImageUsageType::USAGE_TRANSFER_DST_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
            .SharingMode = ESharingMode::EXCLUSIVE,
            .SamplesCount = ESamplesCount::COUNT_1,
            .Flags = EImageCreateFlags::NONE,
            .ViewFormat = EFormat::R8G8B8A8_SRGB,
            .ViewType = EImageViewType::TYPE_2D,
            .ViewAspect = EImageAspect::COLOR_BIT
        };

        SUntypedBuffer textureBuffer(textureData.Width * textureData.Height * textureData.Type, textureData.Data);

        SImageToBufferCopyConfig layoutTransition =
        {
            .FormatBeforeTransition = EFormat::R8G8B8A8_SRGB,
            .LayoutBeforeTransition = EImageLayout::UNDEFINED,
            .FormatAfterTransition = EFormat::R8G8B8A8_SRGB,
            .LayoutAfterTransition = EImageLayout::SHADER_READ_ONLY_OPTIMAL
        };

        ImagePtr image = mDeviceInternal->CreateImageWithData(mCommandPoolInternal, imageConfig, textureBuffer, layoutTransition);

        mImages.insert({ path, image });

        return image;
    }

    void TextureLoader::Deinit()
    {
        LogMessage(psm::MessageSeverity::Info, "TextureLoader destructor");

        mDeviceInternal = nullptr;
        mCommandPoolInternal = nullptr;

        mWhiteTexture = nullptr;
        mImages.clear();
    }
}