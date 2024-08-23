# Compilable SQL
Concept of SQL generation using common C++ syntax and power of compiler's data types checking.

## 1. Create a table

The given table with 6 columns
```
[ ID | C1 | C2 | C3 | C4 | C5 ]
[----|----|----|----|----|----]
```

Where:
* `ID` - it is unique autoicremented row IDs (`PRIMARY_KEY`, `AUTOINCREMENT`)
* `C1` - floating point data with defaul value `3.14`
* `C2` - timestamp with non-null values
* `C3` - string with default value "" (empty string)
* `C4` - floating point rows with default value `5.65`
* `C5` - floating point rows without any extra parameters

Can be created with SQL:
```sql
CREATE TABLE Table1
(
    ID INTEGER PRIMARY KEY AUTOINCREMENT,
    C1 REAL DEFAULT 3.140000,
    C2 DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
    C3 TEXT DEFAULT '',
    C4 REAL DEFAULT 5.650000,
    C5 REAL
);
```

This SQL request can be generated with given C++ snippet:
1. Declate table as a reqular structure:
```cpp
struct Table1: Table
{
    TableColumn<int>          ID = { SqlName("ID"), TCS(TCS::PRIMARY_KEY|TCS::AUTOINCREMENT) };
    TableColumn<double>       C1 = { SqlName("C1"), TCS::DEFAULT, 3.14 };
    TableColumn<std::time_t>  C2 = { SqlName("C2"), TCS(TCS::DEFAULT|TCS::NOT_NULL) };
    TableColumn<std::string>  C3 = { TCS::DEFAULT };
    TableColumn<float>        C4 = { 5.65f };
    TableColumn<float>        C5;

    Table1 (): Table(SqlName("Table1")) { registerColumns(ID, C1, C2, C3, C4, C5); }
};
```
2. Create instance of `Table1` and call method to generate needed SQL request:
```cpp
Table1 table1;
std::string sql = table1.getSqlTableCreate();
```

## 2. Data row insertion

To add two rows with given values:
```
[ ID | C1   | C2         | C3    | C4   | C5    ]
[----|------|------------|-------|------|-------]
[ 0  | 1.23 | 1631730839 | "XXX" | 3.14 | 31.4  ]
[ 1  | 45.6 | 1631730839 | "YYY" | 56.7 | 8.88  ]
[----|------|------------|-------|------|-------]
```

Need SQL request:
```sql
INSERT INTO Table1 ( C1, C2, C3, C4, C5 ) VALUES ( 1.23, 1631730839, 'XXX', 3.140000, 31.4 );
INSERT INTO Table1 ( C1, C2, C3, C4, C5 ) VALUES ( 45.6, 1631730839, 'YYY', 56.7, 8.88 );
```

Can be generated by CSQL:
```cpp
table1.addRow(0, 1.23, time(0), "XXX", 3.14f, 31.4f);
table1.addRow(0, 1.11, time(0), "YYY", 3.13f, 8.88f);

std::string sql = Table1.getSqlRowsInsert();
```
The compiler will check, that data passed in `addRow` fits data types of appopriate columns (fields of `Table1` structure).

## 3. Data selection

### 3.1. Simple selection

To simply select some specific data needed given SQL:
```sql
SELECT ID, C1, C2
FROM table1
```

Can be generated by CSQL:
```cpp
std::cout << table1.getSqlRowsSelect
(
    /* list of columns to select */ { table1.ID, table1.C1, table1.C2 },
);
```

### 3.2. Selection with conditions

To select some specific data with needed conditions it can be used given SQL:
```sql
SELECT ID, C1, C2
FROM table1
WHERE C1 = 3.14 AND C2 > C3 AND C4 IN ( ...some_nested_condition_ )
```

Can be generated by CSQL:
```cpp
std::cout << table1.getSqlRowsSelect
(
    { table1.ID, table1.C1, table1.C2 },
    /* "WHERE" condition */ ( table1.C1 ==  3.14 && table1.C2 > table1.C3 && table1.C4.IN( table2.getSqlRowsSelect() ) ),
);
```
It has a common C-like style with values data type checking and nesting conditions support.

### 3.3. Ordering, limit and top offset

To order selected rows, limit and offset it can be used given SQL:
```sql
SELECT ID, C1, C2
FROM table1
WHERE C1 = 3.14 AND C2 > C3 AND C4 IN ( ...some_nested_condition_ )
ORDER BY C1, C2 ASC, C3 DESC
LIMIT 300
TOP 25
```

Can be generated by CSQL:
```cpp
std::cout << table1.getSqlRowsSelect
(
    { table1.ID, table1.C1, table1.C2 },
    ( table1.C1 ==  3.14 && table1.C2 > table1.C3 && table1.C4.IN( table2.getSqlRowsSelect() ) ),
    /* Ordering parameters       */ { { table1.C1, TRST::DEF  }, { table1.C2, TRST::ASC  }, { table1.C3, TRST::DESC } },
    /* LIMIT selected rows count */ 300,
    /* TOP offset                */ 25
);
```

## 4. CSQL SQLite

There is library CSQL-SQLite that manages data in SQLite database.
Example of using can be found in tests of `libcsql-sqlite` and in demo `src/main.cpp`.

## 5. Links

* More examples can be found:
  * in demo `src/main.cpp` (usage of `SQLite3`) 
  * in tests of both of the libraries `libcsql` and `libcsql-sqlite`.

* Project uses [CppProjectTemplate_VSCodeCMakeTools](https://github.com/R3D9477/CppProjectTemplate_VSCodeCMakeTools)
