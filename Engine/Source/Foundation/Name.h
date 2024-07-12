#pragma once

#include <memory>
#include <string>
#include <cstdint>

namespace psm
{
    namespace foundation
    {
        class Name
        {
        public:
            using ID = uint32_t;

            Name();
            Name(const std::string& name);
            Name(const char* name);
            explicit Name(ID id);
            ~Name();

            Name(const Name& other);
            Name(Name&& other);

            Name& operator=(const Name& other);
            Name& operator=(Name&& other);

            const std::string& ToString() const;
            ID ToId() const;

            bool IsValid() const;

        private:
            ID m_Id;
        };

        inline bool operator==(const Name& lhs, const Name& rhs)
        {
            return lhs.ToId() == rhs.ToId();
        }

        inline bool operator<(const Name& lhs, const Name& rhs)
        {
            return lhs.ToId() < rhs.ToId();
        }

        struct NameHashFunction
        {
            std::size_t operator()(const Name& name) const
            {
                return std::hash<Name::ID>()(name.ToId());
            }
        };
    }
}