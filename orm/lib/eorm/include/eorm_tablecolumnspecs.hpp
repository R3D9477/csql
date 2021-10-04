#pragma once

namespace eorm {
namespace core {

enum TableColumnSpecs
{
    EMPTY,
    PRIMARY_KEY,
    AUTOINCREMENT,
    DEFAULT=4,
    NOT_NULL=8
};

typedef TableColumnSpecs TCS;

constexpr bool is_PRIMARY_KEY     (const TableColumnSpecs specs) { return ( specs & PRIMARY_KEY   ); }
constexpr bool is_AUTOINCREMENT   (const TableColumnSpecs specs) { return ( specs & AUTOINCREMENT ); }
constexpr bool is_NOT_NULL        (const TableColumnSpecs specs) { return ( specs & NOT_NULL      ); }
constexpr bool is_DEFAULT         (const TableColumnSpecs specs) { return ( specs & DEFAULT       ); }

}
}
