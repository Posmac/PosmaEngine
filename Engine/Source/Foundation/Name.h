#pragma once

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

            bool operator==(const Name& other) const;
            bool operator<(const Name& other) const;

            const std::string& ToString() const;
            ID ToId() const;

            bool IsValid() const;

        private:
            ID m_Id;
        };
    }
}