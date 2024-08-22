/** \file */

#pragma once

#include <ctime>
#include <string>
#include <type_traits>

namespace csql
{
namespace core
{

enum TableColumnType
{
    INTEGER,
    REAL,
    DATETIME,
    TEXT
};

typedef TableColumnType TCT;

template <typename T> constexpr TableColumnType getTableColumnType()
{
    if constexpr (std::is_same<T, std::time_t>::value)
        return TableColumnType::DATETIME;
    else if constexpr (std::is_same<T, float>::value)
        return TableColumnType::REAL;
    else if constexpr (std::is_same<T, double>::value)
        return TableColumnType::REAL;
    else if constexpr (std::is_same<T, bool>::value)
        return TableColumnType::INTEGER;
    else if constexpr (std::is_same<T, int>::value)
        return TableColumnType::INTEGER;
    else if constexpr (std::is_same<T, const char *>::value)
        return TableColumnType::TEXT;
    else if constexpr (std::is_same<T, std::string>::value)
        return TableColumnType::TEXT;
    else
        return TableColumnType::TEXT;
}

} // namespace core
} // namespace csql
