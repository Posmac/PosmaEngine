#include "Name.h"

#include "NameRegistry.h"

#include <cassert>

namespace psm
{
    namespace foundation
    {
        Name::Name()
            : m_Id{ NameRegistry::INVALID }
        {

        }

        Name::Name(ID id)
            : m_Id{ id }
        {

        }

        Name::Name(const std::string& string)
            : m_Id{ NameRegistry::INVALID }
        {
            m_Id = NameRegistry::Instance()->ToId(string);
        }

        Name::Name(const char* cString)
            : m_Id{ NameRegistry::INVALID }
        {
            m_Id = NameRegistry::Instance()->ToId(cString);
        }

        Name::~Name()
        {

        }

        Name::Name(const Name& other)
            : m_Id{ other.m_Id }
        {

        }

        Name::Name(Name&& other)
            : m_Id{ other.m_Id }
        {

        }

        Name& Name::operator=(const Name& other)
        {
            m_Id = other.m_Id;
            return *this;
        }

        Name& Name::operator=(Name&& other) 
        {
            m_Id = other.m_Id;
            return *this;
        }

        Name::ID Name::ToId() const
        {
            assert(m_Id != NameRegistry::INVALID);
            return m_Id;
        }

        bool Name::IsValid() const
        {
            return m_Id != NameRegistry::INVALID;
        }

        const std::string& Name::ToString() const
        {
            assert(m_Id != NameRegistry::INVALID);
            return NameRegistry::Instance()->ToString(m_Id);
        }
    }
}