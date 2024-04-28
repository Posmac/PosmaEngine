#pragma once

#include <cstdint>

namespace psm
{
    //struct SUntypedBuffer //mutable
    //{
    //    void* DataPtr;
    //    size_t DataSize;
    //};

    struct SUntypedBuffer //immutable
    {
    public:
        SUntypedBuffer(size_t size, void* data) 
            : mDataPtr(data), mDataSize(size) 
        {

        }

        constexpr auto data() const
        {
            return mDataPtr;
        }

        constexpr auto size() const
        {
            return mDataSize;
        }

    private:
        const void* mDataPtr;
        const size_t mDataSize;
    };
}