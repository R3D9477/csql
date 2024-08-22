/** \file */

#include <iostream>

#include "csql_sqlite.h"

using namespace csql::sqlite;

struct MyTableInfo : SqliteTable
{
    TableColumn<int> ID{{"ID"}, TCS(TCS::PRIMARY_KEY | TCS::AUTOINCREMENT)};
    TableColumn<double> C1{{"C1"}, TCS::DEFAULT, 3.14};
    TableColumn<std::time_t> C2{{"C2"}, TCS(TCS::DEFAULT | TCS::NOT_NULL)};
    TableColumn<std::string> C3{TCS::DEFAULT};
    TableColumn<float> C4{5.65f};
    TableColumn<float> C5;

    MyTableInfo(const SqliteDatabase &db) : SqliteTable({"MyTable"}, db)
    {
        registerColumns(ID, C1, C2, C3, C4, C5);
    }
};

int main()
{
    SqliteDatabase db{"my.db"};
    MyTableInfo tbl{db};

    tbl.createTable();

    tbl.clearRows();
    tbl.selectRows(nullptr, {}, {}, {}, {}, 0, 0,
                   [](const size_t rowsCount) { std::cout << "ROWS_COUNT:" << rowsCount << std::endl; });

    tbl.clearRows();

    tbl.addRow(0, 0.314, time_t(0), "Row 01", 44.1f, 55.23f);
    tbl.addRow(1, 3.140, time_t(0), "Row 02", 44.1f, 55.23f);
    tbl.addRow(2, 31.40, time_t(0), "Row 03", 44.1f, 55.23f);
    tbl.addRow(3, 31.40, time_t(0), "Row 04", 44.1f, 55.23f);
    tbl.addRow(4, 31.40, time_t(0), "Row 05", 44.1f, 55.23f);
    tbl.addRow(5, 31.40, time_t(0), "Row 06", 44.1f, 55.23f);

    tbl.insertRows();

    tbl.clearRows();

    tbl.selectRows(
        [&tbl](const size_t rowIndex) {
            std::cout << "ROW_INDEX [" << rowIndex << "]: ";

            // tbl.getMaxRowsCount()
            // tbl.<column>.getRowsCount()

            std::cout << tbl.ID[rowIndex] << "   ";
            std::cout << tbl.C1[rowIndex] << "   ";
            std::cout << tbl.C2[rowIndex] << "   ";
            std::cout << tbl.C3[rowIndex] << "   ";
            std::cout << tbl.C4[rowIndex] << "   ";
            std::cout << tbl.C5[rowIndex] << "   ";

            std::cout << tbl.C5[100500] << "   ";

            std::cout << std::endl;
        },
        {}, {}, {}, {}, 0, 0, [](const size_t rowsCount) { std::cout << "ROWS_COUNT:" << rowsCount << std::endl; });

    tbl.clearRows();

    tbl.addRow(0, 11.1, time_t(0), "UPDATED ROW", 22.22f, 33.333f);

    tbl.updateRows({tbl.ID > 1 && tbl.ID < 5});

    tbl.clearRows();
    tbl.selectRows(
        [&tbl](const size_t rowIndex) {
            std::cout << "ROW_INDEX [" << rowIndex << "]: ";
            if (rowIndex < tbl.ID.getRowsCount())
                std::cout << tbl.ID[rowIndex] << "   ";
            if (rowIndex < tbl.C1.getRowsCount())
                std::cout << tbl.C1[rowIndex] << "   ";
            if (rowIndex < tbl.C2.getRowsCount())
                std::cout << tbl.C2[rowIndex] << "   ";
            if (rowIndex < tbl.C3.getRowsCount())
                std::cout << tbl.C3[rowIndex] << "   ";
            if (rowIndex < tbl.C4.getRowsCount())
                std::cout << tbl.C4[rowIndex] << "   ";
            if (rowIndex < tbl.C5.getRowsCount())
                std::cout << tbl.C5[rowIndex] << "   ";
            std::cout << std::endl;
        },
        {}, {}, {}, {}, 0, 0, [](const size_t rowsCount) { std::cout << "ROWS_COUNT:" << rowsCount << std::endl; });

    tbl.deleteRows({tbl.ID > 3});

    tbl.clearRows();
    tbl.selectRows(
        [&tbl](const size_t rowIndex) {
            std::cout << "ROW_INDEX [" << rowIndex << "]: ";
            if (rowIndex < tbl.ID.getRowsCount())
                std::cout << tbl.ID.getRowValue(rowIndex) << "   ";
            if (rowIndex < tbl.C1.getRowsCount())
                std::cout << tbl.C1.getRowValue(rowIndex) << "   ";
            if (rowIndex < tbl.C2.getRowsCount())
                std::cout << tbl.C2.getRowValue(rowIndex) << "   ";
            if (rowIndex < tbl.C3.getRowsCount())
                std::cout << tbl.C3.getRowValue(rowIndex) << "   ";
            if (rowIndex < tbl.C4.getRowsCount())
                std::cout << tbl.C4.getRowValue(rowIndex) << "   ";
            if (rowIndex < tbl.C5.getRowsCount())
                std::cout << tbl.C5.getRowValue(rowIndex) << "   ";
            std::cout << std::endl;
        },
        {}, {}, {}, {}, 0, 0, [](const size_t rowsCount) { std::cout << "ROWS_COUNT:" << rowsCount << std::endl; });

    tbl.dropTable();
}
