#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace psm
{
    namespace foundation
    {
        class NameRegistry
        {
        public:
            static const uint32_t INVALID = UINT32_MAX;
            //singleton realization
        public:
            NameRegistry(NameRegistry&) = delete;
            void operator=(const NameRegistry&) = delete;
            static NameRegistry* Instance();

            uint32_t ToId(const std::string& string);
            const std::string& ToString(uint32_t id);

        private:
            NameRegistry();
            static NameRegistry* s_Instance;
        private:
            std::unordered_map<std::string, uint32_t> mNameToId;
            std::vector<std::string> mIdToName;
        };
    }
}
