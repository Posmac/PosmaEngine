#include "TextureLoader.h"

#if defined(RHI_VULKAN)
#include "RHI/Vulkan/CVkImage.h"
#include "RHI/Vulkan/CVkDevice.h"
#endif

#include <iostream>
#include <fstream>

#include "DDS.h"
#include "dxgiformat.h"

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

    TextureLoader::RawTextureData::RawTextureData()
    {
        
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

    ImagePtr TextureLoader::LoadDDSTexture(const std::string& path)
    {
        std::ifstream fstream(path, std::ios::binary | std::ios::ate);
        std::streamsize size = fstream.tellg();
        fstream.seekg(0, std::ios::beg);

        if(!fstream.is_open())
        {
            LogMessage(MessageSeverity::Error, "Failed to open DDS file");
        }

        std::vector<char> buffer(size);
        DWORD* data = nullptr;

        if(fstream.read(buffer.data(), size))
        {
            data = reinterpret_cast<DWORD*>(buffer.data());
        }

        assert(data != nullptr);

        //one of the variants
        if(*data != DirectX::DDS_MAGIC)
        {
            LogMessage(MessageSeverity::Error, "File is not a DDS!");
        }
        data++;

        DirectX::DDS_HEADER* imageData = reinterpret_cast<DirectX::DDS_HEADER*>(data);
        DirectX::DDS_PIXELFORMAT pixelFormat = imageData->ddspf;
        DirectX::DDS_HEADER_DXT10* dx10Header = reinterpret_cast<DirectX::DDS_HEADER_DXT10*>(imageData + 1);
        BYTE* rawImageData = nullptr;
        RawDdsData rawDdsData{};

        if(imageData->size != 124)
        {
            LogMessage(MessageSeverity::Error, "DDS Header size mismatch");
        }

        if(pixelFormat.size != 32)
        {
            LogMessage(MessageSeverity::Error, "DDS pixel format size mismatch");
        }

        LogMessage(MessageSeverity::Info, "Image header data: ");
        
        if(imageData->flags & DDS_WIDTH)
        {
            LogMessage(MessageSeverity::Info, "     Image width: " + std::to_string(imageData->width));
            rawDdsData.Width = imageData->width;
        }

        if(imageData->flags & DDS_HEIGHT)
        {
            LogMessage(MessageSeverity::Info, "     Image height: " + std::to_string(imageData->height));
            rawDdsData.Height = imageData->height;
        }

        if(imageData->flags & DDS_HEADER_CAPS)
        {
            //do other things
            LogMessage(MessageSeverity::Info, "     Image caps: ");
            if(imageData->caps & DDS_SURFACE_FLAGS_TEXTURE)
            {
                LogMessage(MessageSeverity::Info, "         Image is DEFAULT TEXTURE");
            }
            if(imageData->caps & DDS_SURFACE_FLAGS_CUBEMAP)
            {
                LogMessage(MessageSeverity::Info, "         Image is COMPLEX ( maybe a CUBEMAP)");
            }
            if(imageData->caps & DDS_SURFACE_FLAGS_MIPMAP)
            {
                LogMessage(MessageSeverity::Info, "         Image is MIPMAPPED");
            }
        }

        bool isCompressed = false;
        if( (imageData->flags & DDS_HEADER_FLAGS_LINEARSIZE) )
        {
            isCompressed = true;
            LogMessage(MessageSeverity::Info, "     Image IS COMPRESSED");
            LogMessage(MessageSeverity::Info, "     Image pitch or linear size: " + std::to_string(imageData->pitchOrLinearSize));
        }

        if((imageData->flags & DDS_HEADER_FLAGS_PITCH))
        {
            isCompressed = false;
            LogMessage(MessageSeverity::Info, "     Image IS UNCOMPRESSED");
            LogMessage(MessageSeverity::Info, "     Image pitch or linear size: " + std::to_string(imageData->pitchOrLinearSize));
        }

        rawDdsData.PitchOrLinearSize = imageData->pitchOrLinearSize;
        rawDdsData.IsCompressed = isCompressed;

        if(imageData->flags & DDS_HEADER_PIXELFORMAT)
        {
            LogMessage(MessageSeverity::Info, "     Image pixel formаt: ");
            LogMessage(MessageSeverity::Info, "         RGBBitCount: " + std::to_string(pixelFormat.RGBBitCount));
            LogMessage(MessageSeverity::Info, "         RBitMask: " + std::to_string(pixelFormat.RBitMask));
            LogMessage(MessageSeverity::Info, "         GBitMask: " + std::to_string(pixelFormat.GBitMask));
            LogMessage(MessageSeverity::Info, "         BBitMask: " + std::to_string(pixelFormat.BBitMask));
            LogMessage(MessageSeverity::Info, "         ABitMask: " + std::to_string(pixelFormat.ABitMask));

            if(pixelFormat.flags & DDS_ALPHAPIXELS)
            {
                LogMessage(MessageSeverity::Info, "         Texture contains alpha data");
            }
            if(pixelFormat.flags & DDS_ALPHA || pixelFormat.flags & DDS_YUV || pixelFormat.flags & DDS_LUMINANCE)
            {
                LogMessage(MessageSeverity::Info, "         Texture OLD DDS file!!");
                DebugBreak();
            }
            if(pixelFormat.flags & DDS_FOURCC)
            {
                //double check for compression
                if(!rawDdsData.IsCompressed)
                    DebugBreak();

                LogMessage(MessageSeverity::Info, "         Texture is COMPRESSED");
                if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', '1', '0'))
                {
                    LogMessage(MessageSeverity::Info, "         Texture format is: " + DXGI_TO_STRING(dx10Header->dxgiFormat));
                    LogMessage(MessageSeverity::Info, "         Texture arraySize is: " + dx10Header->arraySize);
                    DebugBreak();

                    switch(dx10Header->resourceDimension)
                    {
                        case 2:
                            LogMessage(MessageSeverity::Info, "         Texture dimension is: DDS_DIMENSION_TEXTURE1D");
                        case 3:
                            LogMessage(MessageSeverity::Info, "         Texture dimension is: DDS_DIMENSION_TEXTURE2D");
                        case 4:
                            LogMessage(MessageSeverity::Info, "         Texture dimension is: DDS_DIMENSION_TEXTURE3D");
                    }

                    if(dx10Header->miscFlag & DirectX::DDS_RESOURCE_MISC_FLAG::DDS_RESOURCE_MISC_TEXTURECUBE)
                    {
                        rawDdsData.IsCubemap = true;
                        LogMessage(MessageSeverity::Info, "         Image is a CUBEMAP");
                    }

                    rawImageData = reinterpret_cast<BYTE*>(dx10Header + 1);
                }
                else
                {
                    rawImageData = reinterpret_cast<BYTE*>(imageData + 1);

                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '1'))
                    {
                        LogMessage(MessageSeverity::Info, "         Texture format is: DXT1 (BC1)");
                        rawDdsData.Format = EFormat::BC1_RGB_UNORM_BLOCK;
                        rawDdsData.BlockSize = 8;
                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '2'))
                    {
                        LogMessage(MessageSeverity::Info, "         Texture format is: DXT2(BC2)");
                        rawDdsData.Format = EFormat::BC2_UNORM_BLOCK;
                        rawDdsData.BlockSize = 16;
                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '3'))
                    {
                        LogMessage(MessageSeverity::Info, "         Texture format is: DXT3(BC2)");
                        rawDdsData.Format = EFormat::BC2_UNORM_BLOCK;
                        rawDdsData.BlockSize = 16;

                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '4'))
                    {
                        LogMessage(MessageSeverity::Info, "         Texture format is: DXT4(BC3)");
                        rawDdsData.Format = EFormat::BC3_UNORM_BLOCK;
                        rawDdsData.BlockSize = 16;

                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '5'))
                    {
                        LogMessage(MessageSeverity::Info, "         Texture format is: DXT5(BC3)");
                        rawDdsData.Format = EFormat::BC3_UNORM_BLOCK;
                        rawDdsData.BlockSize = 16;

                    }
                }
            }

            if(pixelFormat.flags & DDS_RGB)
            {
                if(rawDdsData.IsCompressed)
                    DebugBreak();

                rawDdsData.BitsPerPixel = pixelFormat.RGBBitCount;

                LogMessage(MessageSeverity::Info, "         Texture is UNCOMPRESSED");
            }
        }

        if(imageData->flags & DDS_HEADER_FLAGS_MIPMAP)
        {
            LogMessage(MessageSeverity::Info, "     Image mips count: " + std::to_string(imageData->mipMapCount));
            rawDdsData.MipsCount = imageData->mipMapCount;
        }

        rawDdsData.Depth = 1;
        if(imageData->flags & DDS_HEADER_FLAGS_VOLUME)
        {
            LogMessage(MessageSeverity::Info, "     Image depth: " + std::to_string(imageData->depth));
            rawDdsData.Depth = imageData->depth;
        }

        if(rawImageData == nullptr)
            DebugBreak();

        rawDdsData.Mip0Data = rawImageData;

        if(rawDdsData.IsCompressed)
            rawDdsData.RowPitch = max(1, ((rawDdsData.Width + 3) / 4)) * rawDdsData.BlockSize;
        else
            rawDdsData.RowPitch = (rawDdsData.Width * rawDdsData.BitsPerPixel + 7) / 8;

        //create RHI image
        SImageConfig imageConfig =
        {
            .ImageSize = { (uint32_t)rawDdsData.Width, (uint32_t)rawDdsData.Height, 1 },
            .MipLevels = static_cast<int>(1), //implement later
            .ArrayLevels = 1,
            .Type = EImageType::TYPE_2D,
            .Format = rawDdsData.Format,
            .Tiling = EImageTiling::OPTIMAL,
            .InitialLayout = EImageLayout::UNDEFINED,
            .Usage = EImageUsageType::USAGE_TRANSFER_SRC_BIT | EImageUsageType::USAGE_TRANSFER_DST_BIT | EImageUsageType::USAGE_SAMPLED_BIT,
            .SharingMode = ESharingMode::EXCLUSIVE,
            .SamplesCount = ESamplesCount::COUNT_1,
            .Flags = EImageCreateFlags::NONE,
            .ViewFormat = rawDdsData.Format,
            .ViewType = EImageViewType::TYPE_2D,
            .ViewAspect = EImageAspect::COLOR_BIT
        };

        SUntypedBuffer textureBuffer(rawDdsData.PitchOrLinearSize, rawDdsData.Mip0Data);

        SImageToBufferCopyConfig layoutTransition =
        {
            .FormatBeforeTransition = rawDdsData.Format,
            .LayoutBeforeTransition = EImageLayout::UNDEFINED,
            .FormatAfterTransition = rawDdsData.Format,
            .LayoutAfterTransition = EImageLayout::SHADER_READ_ONLY_OPTIMAL
        };

        ImagePtr image = mDeviceInternal->CreateImageWithData(mCommandPoolInternal, imageConfig, textureBuffer, layoutTransition);

        mImages.insert({ path, image });

        return image;
    }
}