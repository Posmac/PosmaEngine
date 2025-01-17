#pragma once

#include "Foundation/Name.h"
#include "RHI/Interface/Buffer.h"
#include "RHI/Interface/Image.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    namespace graph
    {
        using BufferHash = std::unordered_map<foundation::Name, BufferPtr, foundation::NameHashFunction>;
        using ImageHash = std::unordered_map<foundation::Name, ImagePtr, foundation::NameHashFunction>;
        using SamplerHash = std::unordered_map<foundation::Name, SamplerPtr, foundation::NameHashFunction>;
        using DescriptorSetLayoutHash = std::unordered_map<foundation::Name, DescriptorSetLayoutPtr, foundation::NameHashFunction>;
        using DescriptorSetHash = std::unordered_map<foundation::Name, DescriptorSetPtr, foundation::NameHashFunction>;
        using PipelineHash = std::unordered_map<foundation::Name, PipelinePtr, foundation::NameHashFunction>;
    }
}