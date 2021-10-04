/** \file */

#pragma once

#include <ctime>
#include <iostream>

#include "eorm_strutils.hpp"

namespace eorm {
namespace core {

template <typename T>
constexpr T getDefaultValue()
{
    if constexpr (std::is_same<T, const char*>::value || std::is_same<T, std::string>::value)
        return "";
    else
        return T{0};
}

template<typename T>
class DefaultValue
{
    const T value{getDefaultValue<T>()};
    const bool is_null{true};

public:

    template <typename T1=T,
        std::enable_if_t<
           !std::is_same<T1, bool>::value,
        bool> = true>
    constexpr DefaultValue (const bool not_null = false): is_null{!not_null}
    { }

    template <typename T1=T,
        std::enable_if_t<
            std::is_arithmetic<T1>::value,
        bool> = true>
    constexpr DefaultValue (const T1& val): value{val},is_null{false}
    { }

    template <typename T1=T,
        std::enable_if_t<
           std::is_same<T1, const char*>::value
        || std::is_same<T1, std::string>::value,
        bool> = true>
    constexpr DefaultValue (const T1& val): value{val},is_null{value == nullptr}
    { }

    template <typename T1=T,
        std::enable_if_t<
            std::is_arithmetic<T1>::value,
        bool> = true>
    constexpr T getValue() const { return value; }

    template <typename T1=T,
        std::enable_if_t<
           std::is_same<T1, const char*>::value
        || std::is_same<T1, std::string>::value,
        bool> = true>
    constexpr const T& getValue() const { return value; }

    constexpr bool isNull() const { return is_null; }

    template <typename T1=T,
        std::enable_if_t<
             std::is_arithmetic<T1>::value
        &&  !std::is_same<T1, time_t>::value,
        bool> = true>
    std::string getSqlValue() const
    {
        std::stringstream out;
        out << ( isNull() ? "NULL" : stringify(value) );
        return out.str();
    }

    template <typename T1=T,
        std::enable_if_t<
            std::is_same<T1, std::time_t>::value,
        bool> = true>
    std::string getSqlValue() const
    {
        std::stringstream out;
        out << ( isNull() ? "NULL" : ( value == time_t{0} ? "CURRENT_TIMESTAMP" : stringify(value) ) );
        return out.str();
    }

    template <typename T1=T,
        std::enable_if_t<
           std::is_same<T1, const char*>::value
        || std::is_same<T1, std::string>::value,
        bool> = true>
    std::string getSqlValue() const
    {
        std::stringstream out;
        out << ( isNull() ? "NULL" : quoted_stringify(value) );
        return out.str();
    }
};

}
}
