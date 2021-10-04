#define BOOST_TEST_MODULE test_sqlgen

#include <boost/test/unit_test.hpp>

#include "eorm_sqlite.h"

using namespace eorm::sqlite;

struct MyTableInfo: SqliteTable
{
    TableColumn<int>          ID { {"ID"}, TCS(TCS::PRIMARY_KEY|TCS::AUTOINCREMENT) };
    TableColumn<double>       C1 { {"C1"}, TCS::DEFAULT, 3.14 };
    TableColumn<std::time_t>  C2 { {"C2"}, TCS(TCS::DEFAULT|TCS::NOT_NULL) };
    TableColumn<std::string>  C3 { TCS::DEFAULT };
    TableColumn<float>        C4 { 5.65f };
    TableColumn<float>        C5;

    MyTableInfo(const SqliteDatabase& db): SqliteTable({"MyTable"}, db)
    {
        registerColumns(ID, C1, C2, C3, C4, C5);
    }
};

BOOST_AUTO_TEST_SUITE(test_eorm)

BOOST_AUTO_TEST_CASE(test_eorm_sqlite_create_and_drop_table)
{
    SqliteDatabase db{"my.db"};

    MyTableInfo tbl{db};

    BOOST_CHECK( !tbl.dropTable() );

    BOOST_CHECK( tbl.createTable() );

    BOOST_CHECK( !tbl.createTable() );

    BOOST_CHECK( tbl.dropTable() );

    BOOST_CHECK( !tbl.dropTable() );
}

BOOST_AUTO_TEST_CASE(test_eorm_sqlite_insert_rows)
{
    SqliteDatabase db{"my.db"};

    MyTableInfo tbl{db};
    tbl.dropTable();

    BOOST_CHECK( tbl.createTable() );

    tbl.clearRows();
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 0.314, time_t(0), "Row 01", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(1, 3.140, time_t(0), "Row 02", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(2, 31.40, time_t(0), "Row 03", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(3, 31.40, time_t(0), "Row 04", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(4, 31.40, time_t(0), "Row 05", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(5, 31.40, time_t(0), "Row 06", 44.1f, 55.23f) );

    BOOST_CHECK( tbl.insertRows() );

    BOOST_CHECK( tbl.dropTable() );
}

BOOST_AUTO_TEST_CASE(test_eorm_sqlite_insert_and_count_rows)
{
    SqliteDatabase db{"my.db"};
    MyTableInfo tbl{db};

    BOOST_CHECK( tbl.createTable() );

    tbl.clearRows();
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 0.314, time_t(0), "Row 01", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(1, 3.140, time_t(0), "Row 02", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(2, 31.40, time_t(0), "Row 03", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(3, 31.40, time_t(0), "Row 04", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(4, 31.40, time_t(0), "Row 05", 44.1f, 55.23f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(5, 31.40, time_t(0), "Row 06", 44.1f, 55.23f) );

    BOOST_CHECK( tbl.insertRows() );

    tbl.clearRows();
    tbl.selectRows(
        nullptr,
        {},{},{},{},0,0,
        [](const size_t rowsCount)
        {
            BOOST_CHECK( rowsCount == 6 );
        }
    );

    BOOST_CHECK( tbl.dropTable() );
}

BOOST_AUTO_TEST_CASE(test_eorm_sqlite_insert_and_select_all_rows)
{
    SqliteDatabase db{"my.db"};

    MyTableInfo tbl{db};
    tbl.dropTable();

    BOOST_CHECK( tbl.createTable() );

    tbl.clearRows();
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 1.111, time_t(1631730831), "Row 01", 1.1f, 1.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 2.222, time_t(1631730832), "Row 02", 2.1f, 2.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 3.333, time_t(1631730833), "Row 03", 3.1f, 3.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 4.444, time_t(1631730834), "Row 04", 4.1f, 4.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 5.555, time_t(1631730835), "Row 05", 5.1f, 5.01f) );

    BOOST_CHECK( tbl.insertRows() );

    tbl.clearRows();
    tbl.selectRows(
        [&tbl](const size_t rowIndex)
        {
            BOOST_CHECK(
                   rowIndex < tbl.ID.getRowsCount()
                && rowIndex < tbl.C1.getRowsCount()
                && rowIndex < tbl.C2.getRowsCount()
                && rowIndex < tbl.C3.getRowsCount()
                && rowIndex < tbl.C4.getRowsCount()
                && rowIndex < tbl.C5.getRowsCount()
            );

            switch (rowIndex)
            {
                case 0:
                    BOOST_CHECK( tbl.ID[rowIndex] == 1 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 1.111 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730831) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 01" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 1.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 1.01f );
                    return;
                case 1:
                    BOOST_CHECK( tbl.ID[rowIndex] == 2 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 2.222 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730832) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 02" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 2.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 2.01f );
                    return;
                case 2:
                    BOOST_CHECK( tbl.ID[rowIndex] == 3 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 3.333 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730833) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 03" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 3.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 3.01f );
                    return;
                case 3:
                    BOOST_CHECK( tbl.ID[rowIndex] == 4 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 4.444 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730834) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 04" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 4.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 4.01f );
                    return;
                case 4:
                    BOOST_CHECK( tbl.ID[rowIndex] == 5 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 5.555 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730835) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 05" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 5.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 5.01f );
                    return;
            }
            BOOST_CHECK( false );
        },
        {},
        {},
        {},
        {},
        0,
        0,
        [](const size_t rowsCount)
        {
            BOOST_CHECK( rowsCount == 5 );
        }
    );

    BOOST_CHECK( tbl.dropTable() );
}

BOOST_AUTO_TEST_CASE(test_eorm_sqlite_insert_and_select_rows)
{
    SqliteDatabase db{"my.db"};

    MyTableInfo tbl{db};
    tbl.dropTable();

    BOOST_CHECK( tbl.createTable() );

    tbl.clearRows();
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 1.111, time_t(1631730831), "Row 01", 1.1f, 1.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 2.222, time_t(1631730832), "Row 02", 2.1f, 2.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 3.333, time_t(1631730833), "Row 03", 3.1f, 3.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 4.444, time_t(1631730834), "Row 04", 4.1f, 4.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 5.555, time_t(1631730835), "Row 05", 5.1f, 5.01f) );

    BOOST_CHECK( tbl.insertRows() );

    tbl.clearRows();
    tbl.selectRows(
        [&tbl](const size_t rowIndex)
        {
            BOOST_CHECK(
                   rowIndex < tbl.ID.getRowsCount()
                && rowIndex < tbl.C1.getRowsCount()
                && rowIndex < tbl.C2.getRowsCount()
                && rowIndex < tbl.C3.getRowsCount()
                && rowIndex < tbl.C4.getRowsCount()
                && rowIndex < tbl.C5.getRowsCount()
            );

            switch (rowIndex)
            {
                case 0:
                    BOOST_CHECK( tbl.ID[rowIndex] == 2 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 2.222 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730832) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 02" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 2.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 2.01f );
                    return;
                case 1:
                    BOOST_CHECK( tbl.ID[rowIndex] == 3 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 3.333 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730833) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 03" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 3.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 3.01f );
                    return;
                case 2:
                    BOOST_CHECK( tbl.ID[rowIndex] == 4 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 4.444 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730834) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 04" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 4.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 4.01f );
                    return;
            }
            BOOST_CHECK( false );
        },
        {},
        {},
        {
            tbl.ID > 1 && tbl.ID < 5
        },
        {},
        0,
        0,
        [](const size_t rowsCount)
        {
            BOOST_CHECK( rowsCount == 3 );
        }
    );

    BOOST_CHECK( tbl.dropTable() );
}

BOOST_AUTO_TEST_CASE(test_eorm_sqlite_update_and_select_rows)
{
    SqliteDatabase db{"my.db"};

    MyTableInfo tbl{db};
    tbl.dropTable();

    BOOST_CHECK( tbl.createTable() );

    tbl.clearRows();
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 1.111, time_t(1631730831), "Row 01", 1.1f, 1.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 2.222, time_t(1631730832), "Row 02", 2.1f, 2.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 3.333, time_t(1631730833), "Row 03", 3.1f, 3.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 4.444, time_t(1631730834), "Row 04", 4.1f, 4.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 5.555, time_t(1631730835), "Row 05", 5.1f, 5.01f) );

    BOOST_CHECK( tbl.insertRows() );

    tbl.clearRows();
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 888.888, time_t(0), "UPDATED ROW", 8.88f, 88.008f) );

    BOOST_CHECK( tbl.updateRows({ tbl.ID > 1 && tbl.ID < 5 }) );

    tbl.clearRows();
    tbl.selectRows(
        [&tbl](const size_t rowIndex)
        {
            BOOST_CHECK(
                   rowIndex < tbl.ID.getRowsCount()
                && rowIndex < tbl.C1.getRowsCount()
                && rowIndex < tbl.C2.getRowsCount()
                && rowIndex < tbl.C3.getRowsCount()
                && rowIndex < tbl.C4.getRowsCount()
                && rowIndex < tbl.C5.getRowsCount()
            );

            switch (rowIndex)
            {
                case 0:
                    BOOST_CHECK( tbl.ID[rowIndex] == int(rowIndex+1) );
                    BOOST_CHECK( tbl.C1[rowIndex] == 1.111 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730831) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 01" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 1.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 1.01f );
                    return;
                case 1: case 2: case 3:
                    BOOST_CHECK( tbl.ID[rowIndex] == int(rowIndex+1) );
                    BOOST_CHECK( tbl.C1[rowIndex] == 888.888 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(0) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "UPDATED ROW" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 8.88f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 88.008f );
                    return;
                case 4:
                    BOOST_CHECK( tbl.ID[rowIndex] == int(rowIndex+1) );
                    BOOST_CHECK( tbl.C1[rowIndex] == 5.555 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730835) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 05" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 5.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 5.01f );
                    return;
            }
            BOOST_CHECK( false );
        },
        {},
        {},
        {},
        {},
        0,
        0,
        [](const size_t rowsCount)
        {
            BOOST_CHECK( rowsCount == 5 );
        }
    );

    BOOST_CHECK( tbl.dropTable() );
}

BOOST_AUTO_TEST_CASE(test_eorm_sqlite_delete_and_select_rows)
{
    SqliteDatabase db{"my.db"};

    MyTableInfo tbl{db};
    tbl.dropTable();

    BOOST_CHECK( tbl.createTable() );

    tbl.clearRows();
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 1.111, time_t(1631730831), "Row 01", 1.1f, 1.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 2.222, time_t(1631730832), "Row 02", 2.1f, 2.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 3.333, time_t(1631730833), "Row 03", 3.1f, 3.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 4.444, time_t(1631730834), "Row 04", 4.1f, 4.01f) );
    BOOST_REQUIRE_NO_THROW( tbl.addRow(0, 5.555, time_t(1631730835), "Row 05", 5.1f, 5.01f) );

    BOOST_CHECK( tbl.insertRows() );

    BOOST_CHECK( tbl.deleteRows({ tbl.ID > 1 && tbl.ID < 5 }) );

    tbl.clearRows();
    tbl.selectRows(
        [&tbl](const size_t rowIndex)
        {
            BOOST_CHECK(
                   rowIndex < tbl.ID.getRowsCount()
                && rowIndex < tbl.C1.getRowsCount()
                && rowIndex < tbl.C2.getRowsCount()
                && rowIndex < tbl.C3.getRowsCount()
                && rowIndex < tbl.C4.getRowsCount()
                && rowIndex < tbl.C5.getRowsCount()
            );

            switch (rowIndex)
            {
                case 0:
                    BOOST_CHECK( tbl.ID[rowIndex] == 1 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 1.111 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730831) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 01" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 1.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 1.01f );
                    return;
                case 1:
                    BOOST_CHECK( tbl.ID[rowIndex] == 5 );
                    BOOST_CHECK( tbl.C1[rowIndex] == 5.555 );
                    BOOST_CHECK( tbl.C2[rowIndex] == time_t(1631730835) );
                    BOOST_CHECK( tbl.C3[rowIndex] == "Row 05" );
                    BOOST_CHECK( tbl.C4[rowIndex] == 5.1f );
                    BOOST_CHECK( tbl.C5[rowIndex] == 5.01f );
                    return;
            }
            BOOST_CHECK( false );
        },
        {},
        {},
        {},
        {},
        0,
        0,
        [](const size_t rowsCount)
        {
            BOOST_CHECK( rowsCount == 2 );
        }
    );

    BOOST_CHECK( tbl.dropTable() );
}

}
