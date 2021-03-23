#ifndef QSQLITE_H
#define QSQLITE_H

#include "sqlgen.h"

#include <QObject>
#include <QSqlDatabase>

namespace eorm {

    /*class QDBProvider: public QObject
    {
        std::string dbFilePath;
        std::initializer_list<Table*> dbTables;

        QDBProvider(const std::string& dbFilePath_, std::initializer_list<Table*> dbTables_):
            dbFilePath(dbFilePath_), dbTables(dbTables_) {}

        inline Table* operator[] (uint index)
        {
            if (index < this->dbTables.size()) return this->dbTables[index];
        }
    };*/

}

#endif // QSQLITE_H
