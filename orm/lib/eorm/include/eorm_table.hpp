/** \file */

#pragma once

#include <ctime>
#include <memory>
#include <vector>
#include <cassert>
#include <sstream>

#include "eorm_sqlname.hpp"
#include "eorm_sqlexpr.hpp"
#include "eorm_tablecomparsionexpr.hpp"
#include "eorm_tablecolumntype.hpp"
#include "eorm_tablecolumnspecs.hpp"
#include "eorm_exception.hpp"

namespace eorm {
namespace core {

class Table;

class TableColumnBase
{
protected:

    const TCT columnType;
    const TCS columnSpecs;

    SqlName columnName;

    std::shared_ptr<Table> table;

public:

    friend class Table;

    TableColumnBase (const TCT type, const TCS specs = TCS::EMPTY, const SqlName& name = {}):
        columnType{type},
        columnSpecs{specs},
        columnName{name}
    { }

    virtual TableColumnType getType() const { return columnType; }

    virtual SqlExpr getSqlName (const char delimiter = '.') const;
    virtual SqlExpr getSqlDefaultValue () const { return {}; }
    virtual SqlExpr getSqlRowValue (size_t row_index) const { std::ignore = row_index; return {}; }
    virtual SqlExpr getSqlCreateTable () const { return {}; }

    virtual size_t getRowsCount () const { return 0; }
    virtual void clearRows () {}
    virtual void addRowStr (const char* rowStr) { std::ignore = rowStr; }
    virtual void addRowPtr (const std::shared_ptr<void> rowPtr) { std::ignore = rowPtr; }
    virtual void removeLastRowPtr () {}

    virtual bool is_PRIMARY_KEY     () const { return eorm::core::is_PRIMARY_KEY(this->columnSpecs);   }
    virtual bool is_AUTOINCREMENT   () const { return eorm::core::is_AUTOINCREMENT(this->columnSpecs); }
    virtual bool is_NOT_NULL        () const { return eorm::core::is_NOT_NULL(this->columnSpecs);      }
    virtual bool is_DEFAULT         () const { return eorm::core::is_DEFAULT(this->columnSpecs);       }
    virtual bool is_VALUE_REQUIRED  () const { return is_NOT_NULL()||is_DEFAULT(); }

    operator TableColumnComparsionExpr() const
    {
        return
            this->columnName.value == ""
            ? TableColumnComparsionExpr(this->getSqlDefaultValue())
            : TableColumnComparsionExpr(this->getSqlName());
    }

    TableColumnBase& operator= (const TableColumnBase&) = delete;

    TableColumnComparsionExpr operator> (const SqlName& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) > TableColumnComparsionExpr(second);   }

    TableColumnComparsionExpr operator< (const SqlName& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) < TableColumnComparsionExpr(second);   }

    TableColumnComparsionExpr operator== (const SqlName& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) == TableColumnComparsionExpr(second);  }

    TableColumnComparsionExpr operator!= (const SqlName& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) != TableColumnComparsionExpr(second);  }

    TableColumnComparsionExpr IN (const SqlName& second) const
    { return static_cast<TableColumnComparsionExpr>(*this).IN(TableColumnComparsionExpr(second)); }

    TableColumnComparsionExpr operator> (const TableColumnBase& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) > static_cast<TableColumnComparsionExpr>(second);   }

    TableColumnComparsionExpr operator< (const TableColumnBase& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) < static_cast<TableColumnComparsionExpr>(second);   }

    TableColumnComparsionExpr operator== (const TableColumnBase& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) == static_cast<TableColumnComparsionExpr>(second);  }

    TableColumnComparsionExpr operator!= (const TableColumnBase& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) != static_cast<TableColumnComparsionExpr>(second);  }

    TableColumnComparsionExpr IN (const TableColumnBase& second) const
    { return static_cast<TableColumnComparsionExpr>(*this).IN(static_cast<TableColumnComparsionExpr>(second)); }

    TableColumnComparsionExpr IN (const TableColumnComparsionExpr& second) const
    { return static_cast<TableColumnComparsionExpr>(*this).IN(second); }
};

struct TableRowSort;
struct TableJoin;

class Table
{
protected:

    const SqlName tableName;
    std::vector<std::reference_wrapper<TableColumnBase>> tableColumns;

public:

    decltype(tableColumns)::iterator begin() { return tableColumns.begin(); }
    decltype(tableColumns)::iterator end()   { return tableColumns.end();   }
    decltype(tableColumns)::const_iterator begin() const { return tableColumns.cbegin(); }
    decltype(tableColumns)::const_iterator end()   const { return tableColumns.cend();   }

    Table(const SqlName& name) : tableName{name} { }

    template <typename... T>
    void registerColumns(T&&... columns)
    {
        auto this_sp = std::shared_ptr<Table>(this,[](Table*){});
        size_t col_index{0};

        tableColumns =
        {
            (
                (
                    [&this_sp,&col_index](T& tc) -> T&
                    {
                        tc.table = this_sp;
                        if (trim(tc.columnName.value) == "")
                            tc.columnName.value = "COLUMN_" + std::to_string(col_index);
                        ++col_index;
                        return tc;
                    }
                )
                (columns)
            )...
        };
    }

    const SqlName& getTableName() const;

    size_t getMaxRowsCount() const;

    SqlExpr getSqlTableCreate() const;

    SqlExpr getSqlTableDrop() const;

    SqlExpr getSqlRowsInsert(const bool insertValues = true) const;

    SqlExpr getSqlRowsUpdate(
        const TableColumnComparsionExpr& condition   = {},
        const size_t count  = 0,
        const size_t offset = 0) const;

    SqlExpr getSqlRowsSelect(
        const std::initializer_list<TableColumnBase>& sc = {},
        const std::initializer_list<TableJoin>& tableJoins = {},
        const TableColumnComparsionExpr& condition = {},
        const std::initializer_list<TableRowSort>& sort = {},
        const size_t count  = 0,
        const size_t offset = 0) const;

    SqlExpr getSqlRowsDelete(
        const TableColumnComparsionExpr& condition   = {},
        const size_t count  = 0,
        const size_t offset = 0) const;

    SqlExpr getSqlRowsCount(const SqlExpr& sqlEpr) const;

    void clearRows();

    template<typename... T>
    void addRow(T&&... t)
    {
        if (this->tableColumns.size() != sizeof...(T))
            throw ValuesCountException{};

        size_t colBuf = 0;
        (void)std::initializer_list<int>{(addRow(colBuf++, std::forward<T>(t)), void(), 0)...};
    }

    template <typename T>
    void addRow(size_t colIndex, const T val)
    {
        if (colIndex >= this->tableColumns.size())
            throw ValuesCountException{};

        auto buf = stdstr_or_type(val);

        if (getTableColumnType<typeof(buf)>() != this->tableColumns[colIndex].get().columnType)
        {
            std::for_each_n(this->tableColumns.begin(), colIndex, [](auto& column){ column.get().removeLastRowPtr(); });
            throw ValueTypeException{colIndex};
        }

        this->tableColumns[colIndex].get().addRowPtr(std::make_shared<typeof(buf)>(buf));
    }

    Table& operator= (const Table&) = delete;
};

struct TableRowSort
{
    enum TableRowSortType
    {
        DEFAULT,
        ASC,
        DESC
    };

    TableColumnBase& column;
    TableRowSortType sortType;
};

struct TableJoin
{
    enum TableJoinType
    {
        INNER,
        OUTER,
        LEFT,
        RIGHT
    };

    TableJoinType joinType;
    SqlName tableName;
    TableColumnComparsionExpr joinCondition;

    TableJoin(
        const TableJoinType joinType_,
        const SqlName& tableName_,
        const TableColumnComparsionExpr& joinCondition_ = {}):
        joinType{joinType_},
        tableName{tableName_},
        joinCondition{joinCondition_}
    { }

    TableJoin(
        const TableJoinType joinType_,
        const Table& table_,
        const TableColumnComparsionExpr& joinCondition_ = {}):
        TableJoin{joinType_,table_.getTableName(),joinCondition_}
    { }

    friend std::ostream& operator<< (std::ostream& os, const TableJoin& tableJoin)
    {
        switch (tableJoin.joinType)
        {
            case INNER: os << "INNER" ; break;
            case OUTER: os << "OUTER" ; break;
            case LEFT:  os << "LEFT"  ; break;
            case RIGHT: os << "RIGHT" ; break;
            default: return os;
        }
        os << " JOIN " << tableJoin.tableName;
        if (!tableJoin.joinCondition.isEmpty())
            os << " ON " << tableJoin.joinCondition;
        return os;
    }
};

typedef TableRowSort                     TRS;
typedef TableJoin::TableJoinType         TJT;
typedef TableRowSort::TableRowSortType   TRST;

}
}
