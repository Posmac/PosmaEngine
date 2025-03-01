#pragma once

#include <memory>

#include "Include/vulkan/vulkan.h"

#include "RHI/Interface/Types.h"
#include "RHI/Configs/TextureConfig.h"

#include "RHI/Interface/Image.h"

namespace psm
{
    class CVkImage : public IImage, std::enable_shared_from_this<CVkImage>
    {
    public:
        CVkImage(const DevicePtr& device, const SImageConfig& config);
        virtual ~CVkImage();
    public:
        virtual EImageType GetImageType() const override;
        virtual EFormat GetImageFormat() const override;
        virtual SResourceExtent3D GetImageSize() const override;
        virtual uint32_t GetImageWidth() const override;
        virtual uint32_t GetImageHeight() const override;
        virtual uint32_t GetImageDepth() const override;
        virtual uint32_t GetImageMips() const override;
        virtual uint32_t GetArrayLayers() const override;
        virtual uint32_t GetSampleCount() const override;
        virtual uint32_t SubresourceCount() const override;

        virtual void* Raw() override;
        virtual void* Raw() const override;
        virtual void* RawImageView() override;
        virtual void* RawImageView() const override;

    private:
        uint32_t FindMemoryType(VkPhysicalDevice gpu, uint32_t typeFilter, VkMemoryPropertyFlags props);
        void GenerateMipMaps(void* pMipMapsData);

    private:
        VkDevice mDeviceInternal;

        VkImageCreateInfo mImageInfo;
        SImageConfig mImageConfig;
        VkImage mImage;
        VkDeviceMemory mImageMemory;
        VkImageView mImageView;
    };
}