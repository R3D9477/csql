/** \file */

#pragma once

#include <cstdlib>

#include "csql_defvalue.hpp"
#include "csql_table.hpp"

namespace csql
{
namespace core
{

struct NullValue
{
};

template <typename T> class TableColumn : public TableColumnBase
{
    static_assert(!(std::is_same<T, bool>::value && std::is_same<T, int>::value && std::is_same<T, float>::value &&
                    std::is_same<T, double>::value && std::is_same<T, std::time_t>::value &&
                    std::is_same<T, std::string>::value),
                  "unknown data type");

    const DefaultValue<T> columnDefValue{is_VALUE_REQUIRED()};

    std::vector<T> columnData;

public:
    TableColumn(const SqlName &name = {}, const TCS specs = TCS::EMPTY)
        : TableColumnBase{getTableColumnType<T>(), specs, name}
    {
    }

    TableColumn(const TCS specs) : TableColumnBase{getTableColumnType<T>(), specs}
    {
    }

    TableColumn(const SqlName &name, const TCS specs, const T &defValue)
        : TableColumnBase{getTableColumnType<T>(), TCS(specs | TCS::DEFAULT), name}, columnDefValue{defValue}
    {
    }

    TableColumn(const SqlName &name, const T &defValue)
        : TableColumnBase{getTableColumnType<T>(), TCS::DEFAULT, name}, columnDefValue{defValue}
    {
    }

    TableColumn(const TCS specs, const T &defValue)
        : TableColumnBase{getTableColumnType<T>(), TCS(specs | TCS::DEFAULT), {}}, columnDefValue{defValue}
    {
    }

    TableColumn(const T &defValue)
        : TableColumnBase{getTableColumnType<T>(), TCS::DEFAULT, {}}, columnDefValue{defValue}
    {
    }

    size_t getRowsCount() const override
    {
        return this->columnData.size();
    }

    void clearRows() override
    {
        this->columnData.clear();
    }

    void addRowStr(const char *rowStr) override
    {
        if (rowStr)
        {
            if constexpr (std::is_same<T, bool>::value || std::is_same<T, int>::value)
                addRowPtr(std::make_shared<T>(T(std::stoi(rowStr))));
            else if constexpr (std::is_floating_point<T>::value)
                addRowPtr(std::make_shared<T>(T(std::stod(rowStr))));
            else if constexpr (std::is_same<T, std::time_t>::value)
                addRowPtr(std::make_shared<T>(T(std::stol(rowStr))));
            else if constexpr (std::is_same<T, std::string>::value)
                addRowPtr(std::make_shared<T>(T(rowStr)));
        }
    }

    void addRowPtr(const std::shared_ptr<void> rowPtr) override
    {
        if (rowPtr)
            this->columnData.push_back(*(std::static_pointer_cast<T>(rowPtr)));
    }

    void removeLastRowPtr() override
    {
        columnData.pop_back();
    }

    template <typename T1 = T, std::enable_if_t<std::is_arithmetic<T1>::value, bool> = true>
    const T getRowValue(const size_t row_index) const
    {
        return row_index < columnData.size() ? columnData[row_index] : columnDefValue.getValue();
    }

    template <
        typename T1 = T,
        std::enable_if_t<std::is_same<T1, const char *>::value || std::is_same<T1, std::string>::value, bool> = true>
    const T &getRowValue(const size_t row_index) const
    {
        return row_index < columnData.size() ? columnData[row_index] : columnDefValue.getValue();
    }

    SqlExpr getSqlDefaultValue() const override
    {
        std::ostringstream sqlBuf;

        sqlBuf << columnDefValue.getSqlValue();

        return {sqlBuf.str()};
    }

    SqlExpr getSqlRowValue(size_t row_index) const override
    {
        return {row_index < columnData.size() ? quoted_stringify(columnData[row_index]) : columnDefValue.getSqlValue()};
    }

    SqlExpr getSqlCreateTable() const override
    {
        std::ostringstream sqlBuf;

        sqlBuf << this->columnName;

        sqlBuf << " ";
        switch (this->columnType)
        {
        case TCT::INTEGER:
            sqlBuf << "INTEGER";
            break;
        case TCT::REAL:
            sqlBuf << "REAL";
            break;
        case TCT::DATETIME:
            sqlBuf << "DATETIME";
            break;
        case TCT::TEXT:
            sqlBuf << "TEXT";
            break;
        default:
            throw "UNKNOWN DATA TYPE";
        }

        if (this->is_PRIMARY_KEY())
            sqlBuf << " " << "PRIMARY KEY";
        if (this->is_AUTOINCREMENT())
            sqlBuf << " " << "AUTOINCREMENT";
        if (this->is_NOT_NULL())
            sqlBuf << " " << "NOT NULL";
        if (this->is_DEFAULT())
            sqlBuf << " " << "DEFAULT";

        if (is_VALUE_REQUIRED())
            sqlBuf << " " << columnDefValue.getSqlValue();

        return {sqlBuf.str()};
    }

    template <typename T1 = T, std::enable_if_t<std::is_arithmetic<T1>::value, bool> = true>
    const T operator[](const size_t row_index) const
    {
        return getRowValue(row_index);
    }

    template <
        typename T1 = T,
        std::enable_if_t<std::is_same<T1, const char *>::value || std::is_same<T1, std::string>::value, bool> = true>
    const T &operator[](const size_t row_index) const
    {
        return getRowValue(row_index);
    }

    TableColumn &operator=(const TableColumn &) = delete;

    TableColumnComparsionExpr operator>(const SqlName &second) const
    {
        return TableColumnBase::operator>(second);
    }
    TableColumnComparsionExpr operator<(const SqlName &second) const
    {
        return TableColumnBase::operator<(second);
    }
    TableColumnComparsionExpr operator==(const SqlName &second) const
    {
        return TableColumnBase::operator==(second);
    }
    TableColumnComparsionExpr operator!=(const SqlName &second) const
    {
        return TableColumnBase::operator!=(second);
    }
    TableColumnComparsionExpr IN(const SqlName &second) const
    {
        return TableColumnBase::IN(second);
    }

    TableColumnComparsionExpr operator>(const TableColumnBase &second) const
    {
        return TableColumnBase::operator>(second);
    }
    TableColumnComparsionExpr operator<(const TableColumnBase &second) const
    {
        return TableColumnBase::operator<(second);
    }
    TableColumnComparsionExpr operator==(const TableColumnBase &second) const
    {
        return TableColumnBase::operator==(second);
    }
    TableColumnComparsionExpr operator!=(const TableColumnBase &second) const
    {
        return TableColumnBase::operator!=(second);
    }
    TableColumnComparsionExpr IN(const TableColumnBase &second) const
    {
        return TableColumnBase::IN(second);
    }

    TableColumnComparsionExpr IN(const TableColumnComparsionExpr &second) const
    {
        return TableColumnBase::IN(second);
    }

    TableColumnComparsionExpr operator>(const T &second) const
    {
        return static_cast<TableColumnComparsionExpr>(*this) >
               static_cast<TableColumnComparsionExpr>(TableColumn{second});
    }

    TableColumnComparsionExpr operator<(const T &second) const
    {
        return static_cast<TableColumnComparsionExpr>(*this) <
               static_cast<TableColumnComparsionExpr>(TableColumn{second});
    }

    TableColumnComparsionExpr operator==(const T &second) const
    {
        return static_cast<TableColumnComparsionExpr>(*this) ==
               static_cast<TableColumnComparsionExpr>(TableColumn{second});
    }

    TableColumnComparsionExpr operator!=(const T &second) const
    {
        return static_cast<TableColumnComparsionExpr>(*this) !=
               static_cast<TableColumnComparsionExpr>(TableColumn{second});
    }

    TableColumnComparsionExpr operator==(const NullValue &) const
    {
        return static_cast<TableColumnComparsionExpr>(*this) == TableColumnComparsionExpr{"NULL"};
    }

    TableColumnComparsionExpr operator!=(const NullValue &) const
    {
        return static_cast<TableColumnComparsionExpr>(*this) != TableColumnComparsionExpr{"NULL"};
    }
};

} // namespace core
} // namespace csql
