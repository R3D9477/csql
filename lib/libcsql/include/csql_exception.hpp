/** \file */

#pragma once

#include <exception>
#include <string>

namespace csql
{
namespace core
{

struct ValuesCountException : public std::exception
{
    const char *what() const throw()
    {
        return "Count of values is not equal to count of table columns.";
    }
};

struct ValueTypeException : public std::exception
{
    const size_t column_index{0};

    ValueTypeException(const size_t column_index_) : column_index{column_index_}
    {
    }

    const char *what() const throw()
    {
        std::stringstream whats;
        whats << "Type of value under index ";
        whats << column_index;
        whats << " is not equal to type of column with same index in table.";
        return whats.str().c_str();
    }
};

} // namespace core
} // namespace csql
