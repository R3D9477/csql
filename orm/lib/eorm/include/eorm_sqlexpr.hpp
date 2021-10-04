/** \file */

#pragma once

#include "eorm_sqlstrdata.hpp"

namespace eorm {
namespace core {

struct SqlExpr: SqlStrData
{
    SqlExpr(const std::string& value_ = ""): SqlStrData{value_} { trim(value); }
};

}
}
