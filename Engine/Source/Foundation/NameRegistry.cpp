#include "NameRegistry.h"

namespace psm
{
    namespace foundation
    {
        NameRegistry* NameRegistry::s_Instance = nullptr;

        NameRegistry* NameRegistry::Instance()
        {
            if(s_Instance == nullptr)
            {
                s_Instance = new NameRegistry();
            }

            return s_Instance;
        }

        uint32_t NameRegistry::ToId(const std::string& string)
        {
            auto found = mNameToId.find(string);

            if(found != mNameToId.end())
            {
                return found->second;
            }

            mIdToName.push_back(string);
            mNameToId.insert({ string, mIdToName.size() - 1 });

            return (uint32_t)mIdToName.size() - 1;
        }

        const std::string& NameRegistry::ToString(uint32_t id)
        {
            return mIdToName.at(id);
        }
    }
}