/** \file */

#pragma once

#include "eorm_sqlstrdata.hpp"

namespace eorm {
namespace core {

struct SqlName: SqlStrData
{
    SqlName(const std::string& value_ = ""): SqlStrData{value_} { trim(value); }
};

}
}
