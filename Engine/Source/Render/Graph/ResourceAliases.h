#pragma once

#include "Foundation/Name.h"
#include "RHI/Interface/Buffer.h"
#include "RHI/Interface/Image.h"
#include "RHI/Interface/Types.h"

namespace psm
{
    namespace graph
    {
        struct NameHashFunction
        {
            size_t operator()(const foundation::Name& name) const
            {
                size_t rowHash = std::hash<int>()(name.ToId());
                size_t colHash = std::hash<std::string>()(name.ToString()) << 1;
                return rowHash ^ colHash;
            }
        };

        using BufferHash = std::unordered_map<foundation::Name, BufferPtr, NameHashFunction>;
        using ImageHash = std::unordered_map<foundation::Name, ImagePtr, NameHashFunction>;
        using SamplerHash = std::unordered_map<foundation::Name, SamplerPtr, NameHashFunction>;
        using DescriptorSetLayoutHash = std::unordered_map<foundation::Name, DescriptorSetLayoutPtr, NameHashFunction>;
        using DescriptorSetHash = std::unordered_map<foundation::Name, DescriptorSetPtr, NameHashFunction>;
    }
}