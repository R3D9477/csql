/** \file */

#pragma once

#include "csql_sqlstrdata.hpp"

namespace csql
{
namespace core
{

struct SqlName : SqlStrData
{
    SqlName(const std::string &value_ = "") : SqlStrData{value_}
    {
        trim(value);
    }
};

} // namespace core
} // namespace csql
