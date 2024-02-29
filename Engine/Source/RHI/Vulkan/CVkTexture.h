#pragma once

#include <memory>

#include "RHI/Interface/Types.h"
#include "RHI/Interface/Image.h"

#include "RHI/Configs/TextureConfig.h"

#include "Include/vulkan/vulkan.h"

namespace psm
{
    class CVkImage : public IImage, std::enable_shared_from_this<CVkImage>
    {
    public:
        CVkImage(DevicePtr device, const SImageConfig& config);
        virtual ~CVkImage();

        virtual EImageType GetImageType() const override;
        virtual EImageFormat GetImageFormat() const override;
        virtual SResourceExtent3D GetImageSize() const override;
        virtual uint32_t GetImageWidth() const override;
        virtual uint32_t GetImageHeight() const override;
        virtual uint32_t GetImageDepth() const override;
        virtual uint32_t GetImageMips() const override;
        virtual uint32_t GetArrayLayers() const override;
        virtual uint32_t GetSampleCount() const override;

    private:
        VkImage mImage;
        VkImageView mImageView;
        VkDeviceMemory mImageMemory;
    };
}