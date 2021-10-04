#include "eorm_table.hpp"

#include <iostream>

namespace eorm {
namespace core {

SqlExpr TableColumnBase::getSqlName (const char delimiter) const
{
    std::ostringstream sqlBuf;

    std::ignore= delimiter;

    if (this->table != nullptr && delimiter != '\0')
        sqlBuf << this->table->getTableName() << delimiter;
    sqlBuf << this->columnName;

    return { sqlBuf.str() };
}

const SqlName& Table::getTableName() const { return this->tableName; }

size_t Table::getMaxRowsCount() const
{
    size_t rows = 0;
    for (auto& cl: this->tableColumns)
        if (rows < cl.get().getRowsCount())
            rows = cl.get().getRowsCount();
    return rows;
}

SqlExpr Table::getSqlTableCreate() const
{
    std::ostringstream sqlBuf;

    sqlBuf << "CREATE TABLE " << this->tableName << " (";

    for (auto& c: tableColumns)
        sqlBuf << std::endl << c.get().getSqlCreateTable() << ',';

    sqlBuf.seekp(-1, sqlBuf.cur);
    sqlBuf << std::endl << ");";

    return { sqlBuf.str() };
}

SqlExpr Table::getSqlTableDrop() const
{
    std::ostringstream sqlBuf;
    sqlBuf << "DROP TABLE " << this->tableName << ';';
    return { sqlBuf.str() };
}

SqlExpr Table::getSqlRowsInsert(const bool insertValues) const
{
    std::ostringstream sqlBuf, sqlBufValues;

    for (size_t rowIndex = 0; rowIndex < this->getMaxRowsCount(); ++rowIndex)
    {
        sqlBuf << "INSERT INTO " << this->tableName << " (";
        sqlBufValues << "VALUES (";

        for (auto& c: tableColumns)
            if (!c.get().is_AUTOINCREMENT())
            {
                sqlBuf << std::endl << c.get().getSqlName('\0') << ',';
                sqlBufValues << std::endl;
                if (insertValues) sqlBufValues << c.get().getSqlRowValue(rowIndex);
                else sqlBufValues << ":" << c.get().getSqlName('_') << '_' << rowIndex;
                sqlBufValues << ',';
            }

        sqlBuf.seekp(-1, sqlBuf.cur);
        sqlBuf << std::endl << ')';

        sqlBufValues.seekp(-1, sqlBuf.cur);
        sqlBufValues << std::endl << ')';

        sqlBuf << std::endl << sqlBufValues.str() << ';' << std::endl;

        sqlBufValues.str("");
        sqlBufValues.clear();
    }

    pop_back(sqlBuf);

    return { sqlBuf.str() };
}

SqlExpr Table::getSqlRowsUpdate(
    const TableColumnComparsionExpr& condition,
    const size_t count,
    const size_t offset) const
{
    std::ostringstream sqlBuf;

    for (size_t rowIndex = 0; rowIndex < this->getMaxRowsCount(); ++rowIndex)
    {
        sqlBuf << "UPDATE " << this->tableName << " SET";

        for (auto& c: tableColumns)
            if (!c.get().is_AUTOINCREMENT())
                sqlBuf << std::endl << c.get().getSqlName('\0') << '=' << c.get().getSqlRowValue(rowIndex) << ',';

        sqlBuf.seekp(-1, sqlBuf.cur);

        if (condition.expr != "")
            sqlBuf << std::endl << "WHERE " << condition;

        if (count > 0)
            sqlBuf << std::endl << "LIMIT " << count;

        if (offset > 0)
            sqlBuf << std::endl << "OFFSET " << offset;

        sqlBuf << ';' << std::endl;
    }

    pop_back(sqlBuf);
    return { sqlBuf.str() };
}

SqlExpr Table::getSqlRowsSelect(
    const std::initializer_list<TableColumnBase>& sc,
    const std::initializer_list<TableJoin>& tableJoins,
    const TableColumnComparsionExpr& condition,
    const std::initializer_list<TableRowSort>& sort,
    const size_t count,
    const size_t offset) const
{
    std::ostringstream sqlBuf;

    sqlBuf << "SELECT";

    if (sc.size() > 0)
    {
        for (auto& c: sc)
            sqlBuf << std::endl << c.getSqlName() << ',';
        sqlBuf.seekp(-1, sqlBuf.cur);
    }
    else sqlBuf << std::endl << this->tableName << ".*";

    sqlBuf << std::endl << "FROM " << this->tableName;

    for (auto& join: tableJoins)
        sqlBuf << std::endl << join;

    if (condition.expr != "")
        sqlBuf << std::endl << "WHERE " << condition;

    if (sort.size() > 0)
    {
        sqlBuf << std::endl << "ORDER BY";
        for (auto& s: sort)
        {
            sqlBuf << " " << s.column.getSqlName();
            switch (s.sortType)
            {
                case TableRowSort::TableRowSortType::ASC:
                    sqlBuf << " ASC";
                    break;
                case TableRowSort::TableRowSortType::DESC:
                    sqlBuf << " DESC";
                    break;
                default:
                    break;
            }
            sqlBuf << ',';
        }
        sqlBuf.seekp(-1, sqlBuf.cur);
    }

    if (count > 0)
        sqlBuf << std::endl << "LIMIT " << count;

    if (offset > 0)
        sqlBuf << std::endl << "OFFSET " << offset;

    sqlBuf << ';';

    return { sqlBuf.str() };
}

SqlExpr Table::getSqlRowsDelete(
    const TableColumnComparsionExpr& condition,
    const size_t count,
    const size_t offset) const
{
    std::ostringstream sqlBuf;

    sqlBuf << "DELETE FROM " << this->tableName;

    if (condition.expr != "")
        sqlBuf << std::endl << "WHERE " << condition;

    if (count > 0)
        sqlBuf << std::endl << "LIMIT " << count;

    if (offset > 0)
        sqlBuf << std::endl << "OFFSET " << offset;

    sqlBuf << ';';

    return { sqlBuf.str() };
}

SqlExpr Table::getSqlRowsCount(const SqlExpr& sqlEpr) const
{
    std::ostringstream sqlBuf;

    sqlBuf << "SELECT COUNT(*) FROM (" << std::endl;
    sqlBuf << sqlEpr;
    sqlBuf.seekp(-1, std::ios::end);
    sqlBuf << std::endl << ");";

    return { sqlBuf.str() };
}

void Table::clearRows()
{
    for (auto& cl: tableColumns)
        cl.get().clearRows();
}

}
}
