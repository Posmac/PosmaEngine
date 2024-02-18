#pragma once

namespace psm
{
    struct UntypedBuffer //mutable
    {
        void* DataPtr;
        size_t DataSize;
    };

    struct UntypedBuffer //immutable
    {
    public:
        UntypedBuffer(size_t size, void* data) 
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