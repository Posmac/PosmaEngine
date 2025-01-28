#include "TextureLoader.h"

#if defined(RHI_VULKAN)
#include "RHI/Vulkan/CVkImage.h"
#include "RHI/Vulkan/CVkDevice.h"
#include "RHI/Vulkan/CVkBuffer.h"
#include "RHI/Vulkan/CVkCommandBuffer.h"
#include "RHI/Vulkan/CVkCommandPool.h"
#include "RHI/Vulkan/CVkFence.h"
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
        if(s_Instance == nullptr)
        {
            s_Instance = new TextureLoader();
        }

        return s_Instance;
    }

    TextureLoader::RawTextureData::RawTextureData()
        : Width(0), Height(0), NrChannels(0), Type(RGB_Type::Default), Data(nullptr)
    {

    }

    TextureLoader::RawTextureData::RawTextureData(RGB_Type type)
    {
        Width = Height = NrChannels = 0;
        Data = nullptr;
        Type = type;
    }

    void TextureLoader::RawTextureData::Load(const std::string& path)
    {
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

    void TextureLoader::LoadAllTextures()
    {
        //takes all textures data and loads everything using 1 time submit cmd buff and 1 big storage buffer

        //prepare staging buffer
        //SBufferConfig stagingBufferConfig =
        //{
        //    .Size = mTexturesSize,
        //    .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
        //    .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
        //};

        //BufferPtr stagingBuffer = mDeviceInternal->CreateBuffer(stagingBufferConfig);

        //void* pData;
        //SBufferMapConfig mapping =
        //{
        //    .Size = mTexturesSize,
        //    .Offset = 0,
        //    .pData = &pData,
        //    .Flags = 0,
        //};

        //stagingBuffer->Map(mapping);

        ////copy all textures data into it
        //char* ptr = (char*)pData;

        //for(auto& item : mStbiImages)
        //{
        //    ImageOffset& offset = mImageOffsets[item.first];
        //    memcpy(ptr, offset.Data, offset.Size);
        //    ptr += offset.Size;
        //}

        //stagingBuffer->Unmap();

        //prepare staging buffers (for non dds image format)
        std::vector<BufferPtr> stagingBuffers;
        for(auto& image : mStbiImages)
        {
            ImageOffset& offset = mImageOffsets[image.first];
            SBufferConfig stagingBufferConfig =
            {
                .Size = offset.Size,
                .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
            };

            BufferPtr stagingBuffer = mDeviceInternal->CreateBuffer(stagingBufferConfig);

            void* pData;
            SBufferMapConfig mapping =
            {
                .Size = offset.Size,
                .Offset = 0,
                .pData = &pData,
                .Flags = 0,
            };

            stagingBuffer->Map(mapping);
            memcpy(pData, offset.Data, offset.Size);
            stagingBuffer->Unmap();

            stagingBuffers.push_back(stagingBuffer);
        }
 
        //prepare staging buffers (for dds image format)
        for(auto& image : mDdsImages)
        {
            ImageOffset& offset = mImageOffsets[image.first];
            SBufferConfig stagingBufferConfig =
            {
                .Size = offset.Size,
                .Usage = EBufferUsage::USAGE_TRANSFER_SRC_BIT,
                .MemoryProperties = EMemoryProperties::MEMORY_PROPERTY_HOST_VISIBLE_BIT | EMemoryProperties::MEMORY_PROPERTY_HOST_COHERENT_BIT
            };

            BufferPtr stagingBuffer = mDeviceInternal->CreateBuffer(stagingBufferConfig);

            void* pData;
            SBufferMapConfig mapping =
            {
                .Size = offset.Size,
                .Offset = 0,
                .pData = &pData,
                .Flags = 0,
            };

            stagingBuffer->Map(mapping);
            memcpy(pData, offset.Data, offset.Size);
            stagingBuffer->Unmap();

            stagingBuffers.push_back(stagingBuffer);
        }

        //begin command buffer
        auto commandBuffer = mDeviceInternal->BeginSingleTimeSubmitCommandBuffer(mCommandPoolInternal);

        //write data to each image (other than DDS format images)
        int currentIndex = 0;

        for(auto& stbiImage : mStbiImages)
        {
            ImageOffset& offset = mImageOffsets[stbiImage.first];

            ImagePtr image = mImages[stbiImage.first];

            SImageLayoutTransition beforeLayout =
            {
                .Format = image->GetImageFormat(),
                .OldLayout = EImageLayout::UNDEFINED,
                .NewLayout = EImageLayout::TRANSFER_DST_OPTIMAL,
                .SourceStage = EPipelineStageFlags::TOP_OF_PIPE_BIT,
                .DestinationStage = EPipelineStageFlags::TRANSFER_BIT,
                .SourceMask = EAccessFlags::NONE,
                .DestinationMask = EAccessFlags::TRANSFER_WRITE_BIT,
                .ImageAspectFlags = EImageAspect::COLOR_BIT,
                .MipLevel = 0,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, image, beforeLayout);

            mDeviceInternal->CopyBufferToImage(commandBuffer, stagingBuffers[currentIndex], image, image->GetImageSize(), EImageAspect::COLOR_BIT, EImageLayout::TRANSFER_DST_OPTIMAL);

            if(image->GetImageMips() > 1)
            {
                //GenerateMipMaps(physicalDevice, commandBuffer, *dstImage, VK_FORMAT_R8G8B8A8_SRGB, size.width, size.height, mipLevels);
            }

            SImageLayoutTransition afterLayout =
            {
                .Format = image->GetImageFormat(),
                .OldLayout = EImageLayout::TRANSFER_DST_OPTIMAL, //or can be undefined
                .NewLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .SourceStage = EPipelineStageFlags::TRANSFER_BIT,
                .DestinationStage = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
                .SourceMask = EAccessFlags::TRANSFER_WRITE_BIT,
                .DestinationMask = EAccessFlags::SHADER_READ_BIT,
                .ImageAspectFlags = EImageAspect::COLOR_BIT,
                .MipLevel = 0,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, image, afterLayout);

            currentIndex++;
        }

        //write data to each image (dds format images)
        for(auto& ddsImage : mDdsImages)
        {
            ImageOffset& offset = mImageOffsets[ddsImage.first];

            ImagePtr image = mImages[ddsImage.first];

            SImageLayoutTransition beforeLayout =
            {
                .Format = image->GetImageFormat(),
                .OldLayout = EImageLayout::UNDEFINED,
                .NewLayout = EImageLayout::TRANSFER_DST_OPTIMAL,
                .SourceStage = EPipelineStageFlags::TOP_OF_PIPE_BIT,
                .DestinationStage = EPipelineStageFlags::TRANSFER_BIT,
                .SourceMask = EAccessFlags::NONE,
                .DestinationMask = EAccessFlags::TRANSFER_WRITE_BIT,
                .ImageAspectFlags = EImageAspect::COLOR_BIT,
                .MipLevel = 0,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, image, beforeLayout);

            mDeviceInternal->CopyBufferToImage(commandBuffer, stagingBuffers[currentIndex], image, image->GetImageSize(), EImageAspect::COLOR_BIT, EImageLayout::TRANSFER_DST_OPTIMAL);

            if(image->GetImageMips() > 1)
            {
                //GenerateMipMaps(physicalDevice, commandBuffer, *dstImage, VK_FORMAT_R8G8B8A8_SRGB, size.width, size.height, mipLevels);
            }

            SImageLayoutTransition afterLayout =
            {
                .Format = image->GetImageFormat(),
                .OldLayout = EImageLayout::TRANSFER_DST_OPTIMAL, //or can be undefined
                .NewLayout = EImageLayout::SHADER_READ_ONLY_OPTIMAL,
                .SourceStage = EPipelineStageFlags::TRANSFER_BIT,
                .DestinationStage = EPipelineStageFlags::FRAGMENT_SHADER_BIT,
                .SourceMask = EAccessFlags::TRANSFER_WRITE_BIT,
                .DestinationMask = EAccessFlags::SHADER_READ_BIT,
                .ImageAspectFlags = EImageAspect::COLOR_BIT,
                .MipLevel = 0,
            };

            mDeviceInternal->ImageLayoutTransition(commandBuffer, image, afterLayout);

            currentIndex++;
        }

        mDeviceInternal->SubmitSingleTimeCommandBuffer(mCommandPoolInternal, commandBuffer);

        mStbiImages.clear();
        mDdsImages.clear();
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

        if(mStbiImages.count(path) != 0)
        {
            return mImages[path];
        }

        mStbiImages.insert({ path, {Rgb_alpha} });
        RawTextureData& textureData = mStbiImages[path];
        textureData.Load(path);
        if(textureData.Data == nullptr)
        {
            LogMessage(MessageSeverity::Error, "Raw texture data pointer is null" + path);
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

        ImagePtr emptyImage = mDeviceInternal->CreateImage(imageConfig);
        mImages.insert({ path, emptyImage });

        ImageOffset offset = { 0, textureData.Width * textureData.Height * textureData.Type, textureData.Data };
        mImageOffsets.insert({ path, offset });

        return emptyImage;
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
        if(mImages.count(path) != 0)
        {
            return mImages[path];
        }

        if(mDdsImages.count(path) != 0)
        {
            return mImages[path];
        }
        
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

        mDdsImages.insert({});
        RawDdsData& rawDdsData = mDdsImages[path];

        if(imageData->size != 124)
        {
            LogMessage(MessageSeverity::Error, "DDS Header size mismatch");
        }

        if(pixelFormat.size != 32)
        {
            LogMessage(MessageSeverity::Error, "DDS pixel format size mismatch");
        }

        //LogMessage(MessageSeverity::Info, "Image header data: ");

        if(imageData->flags & DDS_WIDTH)
        {
            //LogMessage(MessageSeverity::Info, "     Image width: " + std::to_string(imageData->width));
            rawDdsData.Width = imageData->width;
        }

        if(imageData->flags & DDS_HEIGHT)
        {
            //LogMessage(MessageSeverity::Info, "     Image height: " + std::to_string(imageData->height));
            rawDdsData.Height = imageData->height;
        }

        if(imageData->flags & DDS_HEADER_CAPS)
        {
            //do other things
            //LogMessage(MessageSeverity::Info, "     Image caps: ");
            if(imageData->caps & DDS_SURFACE_FLAGS_TEXTURE)
            {
                //LogMessage(MessageSeverity::Info, "         Image is DEFAULT TEXTURE");
            }
            if(imageData->caps & DDS_SURFACE_FLAGS_CUBEMAP)
            {
                //LogMessage(MessageSeverity::Info, "         Image is COMPLEX ( maybe a CUBEMAP)");
            }
            if(imageData->caps & DDS_SURFACE_FLAGS_MIPMAP)
            {
                //LogMessage(MessageSeverity::Info, "         Image is MIPMAPPED");
            }
        }

        bool isCompressed = false;
        if((imageData->flags & DDS_HEADER_FLAGS_LINEARSIZE))
        {
            isCompressed = true;
            //LogMessage(MessageSeverity::Info, "     Image IS COMPRESSED");
            //LogMessage(MessageSeverity::Info, "     Image pitch or linear size: " + std::to_string(imageData->pitchOrLinearSize));
        }

        if((imageData->flags & DDS_HEADER_FLAGS_PITCH))
        {
            isCompressed = false;
            //LogMessage(MessageSeverity::Info, "     Image IS UNCOMPRESSED");
            //LogMessage(MessageSeverity::Info, "     Image pitch or linear size: " + std::to_string(imageData->pitchOrLinearSize));
        }

        rawDdsData.PitchOrLinearSize = imageData->pitchOrLinearSize;
        rawDdsData.IsCompressed = isCompressed;

        if(imageData->flags & DDS_HEADER_PIXELFORMAT)
        {
            //LogMessage(MessageSeverity::Info, "     Image pixel formаt: ");
            //LogMessage(MessageSeverity::Info, "         RGBBitCount: " + std::to_string(pixelFormat.RGBBitCount));
            //LogMessage(MessageSeverity::Info, "         RBitMask: " + std::to_string(pixelFormat.RBitMask));
            //LogMessage(MessageSeverity::Info, "         GBitMask: " + std::to_string(pixelFormat.GBitMask));
            //LogMessage(MessageSeverity::Info, "         BBitMask: " + std::to_string(pixelFormat.BBitMask));
            //LogMessage(MessageSeverity::Info, "         ABitMask: " + std::to_string(pixelFormat.ABitMask));

            if(pixelFormat.flags & DDS_ALPHAPIXELS)
            {
                //LogMessage(MessageSeverity::Info, "         Texture contains alpha data");
            }
            if(pixelFormat.flags & DDS_ALPHA || pixelFormat.flags & DDS_YUV || pixelFormat.flags & DDS_LUMINANCE)
            {
                //LogMessage(MessageSeverity::Info, "         Texture OLD DDS file!!");
                DebugBreak();
            }
            if(pixelFormat.flags & DDS_FOURCC)
            {
                //double check for compression
                if(!rawDdsData.IsCompressed)
                    DebugBreak();

                //LogMessage(MessageSeverity::Info, "         Texture is COMPRESSED");
                if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', '1', '0'))
                {
                    //LogMessage(MessageSeverity::Info, "         Texture format is: " + DXGI_TO_STRING(dx10Header->dxgiFormat));
                    //LogMessage(MessageSeverity::Info, "         Texture arraySize is: " + dx10Header->arraySize);
                    DebugBreak();

                    /*switch(dx10Header->resourceDimension)
                    {
                        case 2:
                            LogMessage(MessageSeverity::Info, "         Texture dimension is: DDS_DIMENSION_TEXTURE1D");
                            break;
                        case 3:
                            LogMessage(MessageSeverity::Info, "         Texture dimension is: DDS_DIMENSION_TEXTURE2D");
                            break;
                        case 4:
                            LogMessage(MessageSeverity::Info, "         Texture dimension is: DDS_DIMENSION_TEXTURE3D");
                            break;
                    }*/

                    if(dx10Header->miscFlag & DirectX::DDS_RESOURCE_MISC_FLAG::DDS_RESOURCE_MISC_TEXTURECUBE)
                    {
                        rawDdsData.IsCubemap = true;
                        //LogMessage(MessageSeverity::Info, "         Image is a CUBEMAP");
                    }

                    rawImageData = reinterpret_cast<BYTE*>(dx10Header + 1);
                }
                else
                {
                    rawImageData = reinterpret_cast<BYTE*>(imageData + 1);

                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '1'))
                    {
                        //LogMessage(MessageSeverity::Info, "         Texture format is: DXT1 (BC1)");
                        rawDdsData.Format = EFormat::BC1_RGB_UNORM_BLOCK;
                        rawDdsData.BlockSize = 8;
                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '2'))
                    {
                        //LogMessage(MessageSeverity::Info, "         Texture format is: DXT2(BC2)");
                        rawDdsData.Format = EFormat::BC2_UNORM_BLOCK;
                        rawDdsData.BlockSize = 16;
                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '3'))
                    {
                        //LogMessage(MessageSeverity::Info, "         Texture format is: DXT3(BC2)");
                        rawDdsData.Format = EFormat::BC2_UNORM_BLOCK;
                        rawDdsData.BlockSize = 16;

                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '4'))
                    {
                        //LogMessage(MessageSeverity::Info, "         Texture format is: DXT4(BC3)");
                        rawDdsData.Format = EFormat::BC3_UNORM_BLOCK;
                        rawDdsData.BlockSize = 16;

                    }
                    if(pixelFormat.fourCC == MAKEFOURCC('D', 'X', 'T', '5'))
                    {
                        //LogMessage(MessageSeverity::Info, "         Texture format is: DXT5(BC3)");
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

                //LogMessage(MessageSeverity::Info, "         Texture is UNCOMPRESSED");
            }
        }

        if(imageData->flags & DDS_HEADER_FLAGS_MIPMAP)
        {
            //LogMessage(MessageSeverity::Info, "     Image mips count: " + std::to_string(imageData->mipMapCount));
            rawDdsData.MipsCount = imageData->mipMapCount;
        }

        rawDdsData.Depth = 1;
        if(imageData->flags & DDS_HEADER_FLAGS_VOLUME)
        {
            //LogMessage(MessageSeverity::Info, "     Image depth: " + std::to_string(imageData->depth));
            rawDdsData.Depth = imageData->depth;
        }

        if(rawImageData == nullptr)
            DebugBreak();

        rawDdsData.Mip0Data = (BYTE*)malloc(rawDdsData.PitchOrLinearSize);
        memcpy(rawDdsData.Mip0Data, rawImageData, rawDdsData.PitchOrLinearSize);
        //rawDdsData.Mip0Data = rawImageData;

        if(rawDdsData.IsCompressed)
            rawDdsData.RowPitch = max(1, ((rawDdsData.Width + 3) / 4)) * rawDdsData.BlockSize;
        else
            rawDdsData.RowPitch = (rawDdsData.Width * rawDdsData.BitsPerPixel + 7) / 8;

        if(rawDdsData.MipsCount > 1)
        {
            //leave it nullptr for now, until we will process mip 1-11 levels
            //rawDdsData.Mip1Data = rawDdsData.Mip0Data + rawDdsData.PitchOrLinearSize;
        }

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

        ImagePtr emptyImage = mDeviceInternal->CreateImage(imageConfig);

        mImages.insert({ path, emptyImage });

        ImageOffset offset = { 0, rawDdsData.PitchOrLinearSize, rawDdsData.Mip0Data};
        mImageOffsets.insert({ path, offset });

        return emptyImage;
    }

    TextureLoader::RawDdsData::~RawDdsData()
    {
        if(Mip0Data != nullptr)
        {
            free(Mip0Data);
        }
    }
}