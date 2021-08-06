# eorm
Concept of one-header ORM for C++

# 1. Table declaration
looks like common C-like structure:
```
struct MyTable: Table
{
    TableColumn<int>          ID = { SqlName("ID"), TCS(TCS::PRIMARY_KEY|TCS::AUTOINCREMENT) };
    TableColumn<double>       C1 = { SqlName("C1"), TCS::DEFAULT, 3.14 };
    TableColumn<std::time_t>  C2 = { SqlName("C2"), TCS(TCS::DEFAULT|TCS::NOT_NULL) };
    TableColumn<std::string>  C3 = { TCS::DEFAULT };
    TableColumn<float>        C4 = { 5.65f };
    TableColumn<float>        C5;

    MyTable (): Table(SqlName("MyTable")) { registerColumns(ID, C1, C2, C3, C4, C5); }
};
```

# 2. Data row insertion
has a value type checking
```
...
myTable.addRow(0, 1.23, time(0), "XXX", 3.14f, 31.4f);
myTable.addRow(0, 1.11, time(0), "YYY", 3.13f, 8.88f);
...
```

# 3. Conditions
has a common C-like style (with value type checking) and nesting support:
```
...
std::cout << myTable.getSqlRowsSelect(
    {
        myTable.ID,
        myTable.C1,
        myTable.C2
    },
           myTable.C1 ==  3.14                              //    conditional expression...
        && myTable.C2 >   myTable.C3                        // ...conditional expression...
        && myTable.C3 ==  SqlName("MyOwnColumn")            // ...conditional expression...
        && myTable.C4.IN( myTable.getSqlRowsSelect() )      // ...conditional expression <-- nested condition!
    ,
    {
        { myTable.C1, TRST::DEF  },
        { myTable.C2, TRST::ASC  },
        { myTable.C3, TRST::DESC }
    },
    300,
    25
);
...
```
