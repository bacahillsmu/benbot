#include "Blackboard.hpp"
#include "core/WrappedUnit.hpp"

// ----------------------------------------------------------------------------
void Blackboard::SetKeyValue(std::string key_, int value_)
{
    m_ints[key_] = value_;
}

// ----------------------------------------------------------------------------
void Blackboard::SetKeyValue(std::string key_, float value_)
{
    m_floats[key_] = value_;
}

// ----------------------------------------------------------------------------
void Blackboard::SetKeyValue(std::string key_, double value_)
{
    m_doubles[key_] = value_;
}

// ----------------------------------------------------------------------------
void Blackboard::SetKeyValue(std::string key_, bool value_)
{
    m_bools[key_] = value_;
}

// ----------------------------------------------------------------------------
void Blackboard::SetKeyValue(std::string key_, std::string value_)
{
    m_strings[key_] = value_;
}

// ----------------------------------------------------------------------------
void Blackboard::SetKeyValue(std::string key_, WrappedUnit* value_)
{
    m_wrappedUnits[key_] = value_;
}

// ----------------------------------------------------------------------------
int Blackboard::GetIntFromKey(std::string key_)
{
    if (m_ints.find(key_) == m_ints.end())
    {
        m_ints[key_] = 0;
    }

    return m_ints[key_];
}

// ----------------------------------------------------------------------------
float Blackboard::GetFloatFromKey(std::string key_)
{
    if (m_floats.find(key_) == m_floats.end())
    {
        m_floats[key_] = 0.0f;
    }

    return m_floats[key_];
}

// ----------------------------------------------------------------------------
double Blackboard::GetDoubleFromKey(std::string key_)
{
    if (m_doubles.find(key_) == m_doubles.end())
    {
        m_doubles[key_] = 0.0;
    }

    return m_doubles[key_];
}

// ----------------------------------------------------------------------------
bool Blackboard::GetBoolFromKey(std::string key_)
{
    if (m_bools.find(key_) == m_bools.end())
    {
        m_bools[key_] = false;
    }

    return m_bools[key_];
}

// ----------------------------------------------------------------------------
std::string Blackboard::GetStringFromKey(std::string key_)
{
    if (m_strings.find(key_) == m_strings.end())
    {
        m_strings[key_] = "";
    }

    return m_strings[key_];
}

// ----------------------------------------------------------------------------
WrappedUnit* Blackboard::GetWrappedUnitFromKey(std::string key_)
{
    if (m_wrappedUnits.find(key_) == m_wrappedUnits.end())
    {
        m_wrappedUnits[key_] = nullptr;
    }

    return m_wrappedUnits[key_];
}

// ----------------------------------------------------------------------------
bool Blackboard::HasIntFromKey(const std::string& key_) const
{
    return m_ints.find(key_) != m_ints.end();
}

// ----------------------------------------------------------------------------
bool Blackboard::HasFloatFromKey(const std::string& key_) const
{
    return m_floats.find(key_) != m_floats.end();
}

// ----------------------------------------------------------------------------
bool Blackboard::HasDoubleFromKey(const std::string& key_) const
{
    return m_doubles.find(key_) != m_doubles.end();
}

// ----------------------------------------------------------------------------
bool Blackboard::HasBoolFromKey(const std::string& key_) const
{
    return m_bools.find(key_) != m_bools.end();
}

// ----------------------------------------------------------------------------
bool Blackboard::HasStringFromKey(const std::string& key_) const
{
    return m_strings.find(key_) != m_strings.end();
}

// ----------------------------------------------------------------------------
bool Blackboard::HasWrappedUnitFromKey(const std::string& key_) const
{
    return m_wrappedUnits.find(key_) != m_wrappedUnits.end();
}
