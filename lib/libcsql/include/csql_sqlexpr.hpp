/** \file */

#pragma once

#include "csql_sqlstrdata.hpp"

namespace csql
{
namespace core
{

struct SqlExpr : SqlStrData
{
    SqlExpr(const std::string &value_ = "") : SqlStrData{value_}
    {
        trim(value);
    }
};

} // namespace core
} // namespace csql
