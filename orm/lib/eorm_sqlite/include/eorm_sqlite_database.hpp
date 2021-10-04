#pragma once

#include <string>
#include <functional>

#include <sqlite3.h>

#include "eorm.h"

using namespace eorm::core;

namespace eorm   {
namespace sqlite {

class SqliteRowsHandlerBase
{
public:
    virtual int rowHandler(int columns, char** data, char** names) = 0;
};

class SqliteDatabase
{
    sqlite3* m_db{nullptr};
    bool m_is_opened{false};

public:

    SqliteDatabase() = default;

    SqliteDatabase(const std::string& db_name) { open(db_name); }

    ~SqliteDatabase() { close(); }

    bool open(const std::string& db_name)
    {
        if (!isOpened())
            m_is_opened = (sqlite3_open(db_name.c_str(), &m_db) == 0);

        return m_is_opened;
    }

    void close() { if (isOpened()) sqlite3_close(m_db); }

    bool isOpened() const
    {
        return m_is_opened ? bool(m_db) : false;
    }

    bool executeQueryAsync(const SqlExpr& query, std::shared_ptr<SqliteRowsHandlerBase> rowsHandler = nullptr) const
    {
        auto resCallback = [](void* context, int columns, char** data, char** names)
        {
            if (context && columns && data && names)
                return static_cast<SqliteRowsHandlerBase*>(context)->rowHandler(columns, data, names);
            return -1;
        };

        return isOpened() ?
            (sqlite3_exec(
                m_db,
                query.str().c_str(),
                resCallback,
                ((rowsHandler) ? rowsHandler.get() : nullptr),
                nullptr
            ) == 0)
        : false;
    }

    SqliteDatabase& operator= (const SqliteDatabase&) = delete;
};

}
}
