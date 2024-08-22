#pragma once

#include <iostream>
#include <memory>

#include "csql.h"
#include "csql_sqlite_database.hpp"
#include "csql_sqlite_rows_handler.hpp"

using namespace csql::core;

namespace csql
{
namespace sqlite
{

class SqliteTable : protected Table
{
    const SqliteDatabase &sqliteDatabase;

public:
    using Table::addRow;
    using Table::clearRows;
    using Table::getMaxRowsCount;
    using Table::getTableName;
    using Table::registerColumns;

    SqliteTable(const SqlName &name, const SqliteDatabase &db) : Table{name}, sqliteDatabase{db}
    {
    }

    bool createTable() const
    {
        auto sql = Table::getSqlTableCreate();
        return sqliteDatabase.executeQueryAsync(std::move(sql));
    }

    bool dropTable() const
    {
        auto sql = Table::getSqlTableDrop();
        return sqliteDatabase.executeQueryAsync(std::move(sql));
    }

    bool insertRows(const bool insertValues = true) const
    {
        auto sql = Table::getSqlRowsInsert(insertValues);
        return sqliteDatabase.executeQueryAsync(std::move(sql));
    }

    bool updateRows(const TableColumnComparsionExpr &condition = {}, const size_t count = 0,
                    const size_t offset = 0) const
    {
        auto sql = Table::getSqlRowsUpdate(condition, count, offset);
        return sqliteDatabase.executeQueryAsync(std::move(sql));
    }

    bool countRows(const std::function<void(size_t)> rowsCountHandler, const SqlExpr &sqlExpr) const
    {
        return sqliteDatabase.executeQueryAsync(Table::getSqlRowsCount(sqlExpr),
                                                std::make_shared<SqliteRowsCountHandler>(rowsCountHandler));
    }

    bool selectRows(const std::function<void(size_t)> rowHandler, const std::initializer_list<TableColumnBase> &sc = {},
                    const std::initializer_list<TableJoin> &tableJoins = {},
                    const TableColumnComparsionExpr &condition = {},
                    const std::initializer_list<TableRowSort> &sort = {}, const size_t count = 0,
                    const size_t offset = 0, const std::function<void(size_t)> rowsCountHandler = nullptr) const
    {
        auto sql = Table::getSqlRowsSelect(sc, tableJoins, condition, sort, count, offset);

        if (rowsCountHandler)
            if (!countRows(rowsCountHandler, sql))
                return false;

        return sqliteDatabase.executeQueryAsync(
            std::move(sql), rowHandler
                                ? std::make_shared<SqliteRowsHandler>(std::initializer_list<Table>{*this}, rowHandler)
                                : nullptr);
    }

    bool deleteRows(const TableColumnComparsionExpr &condition = {}, const size_t count = 0,
                    const size_t offset = 0) const
    {
        auto sql = Table::getSqlRowsDelete(condition, count, offset);
        return sqliteDatabase.executeQueryAsync(std::move(sql));
    }
};

} // namespace sqlite
} // namespace csql
