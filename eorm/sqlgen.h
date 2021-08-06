#ifndef EORM_H
#define EORM_H

#include <cassert>
#include <ctime>
#include <memory>
#include <vector>
#include <sstream>
#include <iostream>

template<typename T>
typename std::enable_if<std::is_fundamental<T>::value, std::string>::type  stringify (const T& t) { return std::to_string(t); }

template<typename T>
typename std::enable_if<!std::is_fundamental<T>::value, std::string>::type stringify (const T& t) { return std::string(t); }

template<typename T>
typename std::enable_if<std::is_same<T, const char*>::value, std::string>::type stdstr_or_type (const T t)
{
    std::string buf = ((const char *)(t));
    return buf;
}

template<typename T>
typename std::enable_if<!std::is_same<T, const char*>::value, T>::type stdstr_or_type (const T t) { return t; }

namespace eorm {

struct SqlStrData
{
    std::string value;
    SqlStrData(const std::string& value_ = ""): value(value_) {}
    inline bool operator== (const std::string& op) const { return this->value == op; }
    friend std::ostream& operator<< (std::ostream &out, const SqlStrData& value_) { out << value_.value; return out; }
};
struct SqlName: SqlStrData { SqlName(const std::string& value_ = ""): SqlStrData(value_){} };
struct SqlExpr: SqlStrData { SqlExpr(const std::string& value_ = ""): SqlStrData(value_){} };

struct Table
{
    struct TableColumnBase
    {
        enum TableColumnType { INTEGER, REAL, DATETIME, TEXT };

        template <typename T>
        static TableColumnType getTableColumnType()
        {
                 if (std::is_same<T, std::time_t>::value)   return TableColumnType::DATETIME;
            else if (std::is_same<T, float>::value)         return TableColumnType::REAL;
            else if (std::is_same<T, double>::value)        return TableColumnType::REAL;
            else if (std::is_same<T, bool>::value)          return TableColumnType::INTEGER;
            else if (std::is_same<T, int>::value)           return TableColumnType::INTEGER;
            else if (std::is_same<T, const char *>::value)  return TableColumnType::TEXT;
            else if (std::is_same<T, std::string>::value)   return TableColumnType::TEXT;
            else throw "unknown data type";
        }

        enum TableColumnSpecs { EMPTY, PRIMARY_KEY, AUTOINCREMENT, DEFAULT=4, NOT_NULL=8 };

        struct TableColumnComparsionExpr
        {
            std::string expr;

            TableColumnComparsionExpr (const std::string& expr_ = ""): expr(expr_ == "" ? "" : "(" + expr_ + ")") { }

            TableColumnComparsionExpr (const SqlName& sqlName_): expr(sqlName_.value) { }

            TableColumnComparsionExpr (const SqlExpr& nestedExpr_): TableColumnComparsionExpr(nestedExpr_.value) { }

            TableColumnComparsionExpr __getExpr(const std::string& op) const
            {
                std::ostringstream sqlBuf;
                sqlBuf << op << " " << this->expr;
                return TableColumnComparsionExpr(sqlBuf.str());
            }

            TableColumnComparsionExpr __getExpr(const TableColumnComparsionExpr& second, const std::string& op) const
            {
                std::ostringstream sqlBuf;
                sqlBuf << this->expr << " " << op << " " << second.expr;
                return TableColumnComparsionExpr(sqlBuf.str());
            }

            TableColumnComparsionExpr __getExpr(const std::string& second, const std::string& op) const
            { return __getExpr(TableColumnComparsionExpr(second), op); }

            friend std::ostream& operator<< (std::ostream& os, const TableColumnComparsionExpr& expr)   { os << expr.expr; return os; }

            inline TableColumnComparsionExpr operator>  (const TableColumnComparsionExpr& second) const { return __getExpr(second, ">");   }
            inline TableColumnComparsionExpr operator<  (const TableColumnComparsionExpr& second) const { return __getExpr(second, "<");   }
            inline TableColumnComparsionExpr operator== (const TableColumnComparsionExpr& second) const { return __getExpr(second, "=");   }
            inline TableColumnComparsionExpr operator!= (const TableColumnComparsionExpr& second) const { return __getExpr(second, "<>");  }
            inline TableColumnComparsionExpr operator&& (const TableColumnComparsionExpr& second) const { return __getExpr(second, "AND"); }
            inline TableColumnComparsionExpr operator!  () const { return __getExpr("NOT"); }

            inline TableColumnComparsionExpr IN (const TableColumnComparsionExpr& second) const { return __getExpr(second, "IN");  }
        };

        static bool is_PRIMARY_KEY     (TableColumnSpecs specs) { return ( specs & PRIMARY_KEY   ); }
        static bool is_AUTOINCREMENT   (TableColumnSpecs specs) { return ( specs & AUTOINCREMENT ); }
        static bool is_NOT_NULL        (TableColumnSpecs specs) { return ( specs & NOT_NULL      ); }
        static bool is_DEFAULT         (TableColumnSpecs specs) { return ( specs & DEFAULT       ); }

        std::shared_ptr<Table>   table;

        SqlName             columnName;
        TableColumnType     columnType;
        TableColumnSpecs    columnSpecs;

        virtual SqlExpr getSqlName          (const char delimiter = '.') const
        {
            std::ostringstream sqlBuf;

            if (this->table != nullptr) sqlBuf << this->table->tableName << delimiter;
            sqlBuf << this->columnName;

            return { sqlBuf.str() };
        }
        virtual SqlExpr getSqlRowValue      (size_t rowIndex = 0) const { std::ignore = rowIndex; return {}; }
        virtual SqlExpr getSqlCreateTable   () const { return {}; }

        virtual size_t getRowsCount () const { return 0; }
        virtual void clearRows () {}
        virtual void addRowPtr (const std::shared_ptr<void> rowPtr) { std::ignore = rowPtr; }

        virtual bool is_PRIMARY_KEY     () const { return is_PRIMARY_KEY(this->columnSpecs);   }
        virtual bool is_AUTOINCREMENT   () const { return is_AUTOINCREMENT(this->columnSpecs); }
        virtual bool is_NOT_NULL        () const { return is_NOT_NULL(this->columnSpecs);      }
        virtual bool is_DEFAULT         () const { return is_DEFAULT(this->columnSpecs);       }

        operator TableColumnComparsionExpr() const
        {
            if (this->columnName.value == "") return TableColumnComparsionExpr(this->getSqlRowValue());
            return TableColumnComparsionExpr(this->getSqlName());
        }

        inline TableColumnComparsionExpr operator> (const SqlName& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) > TableColumnComparsionExpr(second);   }

        inline TableColumnComparsionExpr operator< (const SqlName& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) < TableColumnComparsionExpr(second);   }

        inline TableColumnComparsionExpr operator== (const SqlName& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) == TableColumnComparsionExpr(second);  }

        inline TableColumnComparsionExpr operator!= (const SqlName& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) != TableColumnComparsionExpr(second);  }

        inline TableColumnComparsionExpr IN (const SqlName& second) const
        { return static_cast<TableColumnComparsionExpr>(*this).IN(TableColumnComparsionExpr(second)); }

        inline TableColumnComparsionExpr operator> (const TableColumnBase& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) > static_cast<TableColumnComparsionExpr>(second);   }

        inline TableColumnComparsionExpr operator< (const TableColumnBase& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) < static_cast<TableColumnComparsionExpr>(second);   }

        inline TableColumnComparsionExpr operator== (const TableColumnBase& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) == static_cast<TableColumnComparsionExpr>(second);  }

        inline TableColumnComparsionExpr operator!= (const TableColumnBase& second) const
        { return static_cast<TableColumnComparsionExpr>(*this) != static_cast<TableColumnComparsionExpr>(second);  }

        inline TableColumnComparsionExpr IN (const TableColumnBase& second) const
        { return static_cast<TableColumnComparsionExpr>(*this).IN(static_cast<TableColumnComparsionExpr>(second)); }

        inline TableColumnComparsionExpr IN (const TableColumnComparsionExpr& second) const
        { return static_cast<TableColumnComparsionExpr>(*this).IN(second); }
    };

    struct TableRowSort
    {
        enum TableRowSortType { DEF, ASC, DESC };

        TableColumnBase &column;
        TableRowSortType sortType;
    };

    SqlName tableName;
    std::vector<std::shared_ptr<TableColumnBase>> tableColumns;

    template<typename... T>
    Table(const SqlName& name) : tableName(name) { }

    template <typename... T>
    void registerColumns(T&&... columns) { (void)std::initializer_list<int>{(registerColumn((std::forward<T>(columns))), void(), 0)...}; }

    template <typename T>
    void registerColumn(T& tc)
    {
        if (tc.columnName == "") tc.columnName = "COLUMN_" + std::to_string(tableColumns.size());
        tc.table = std::shared_ptr<Table>(this,[](Table*){});
        tableColumns.push_back(std::shared_ptr<T>(&tc,[](T*){}));
    }

    SqlExpr getSqlTableCreate() const
    {
        std::ostringstream sqlBuf;

        sqlBuf << "CREATE TABLE " << this->tableName << " (";

        for (auto& c: tableColumns)
            sqlBuf << std::endl << c->getSqlCreateTable() << ",";

        sqlBuf.seekp(-1, sqlBuf.cur);
        sqlBuf << std::endl << ");";

        return { sqlBuf.str() };
    }

    SqlExpr getSqlTableDrop() const
    {
        std::ostringstream sqlBuf;
        sqlBuf << "DROP TABLE " << this->tableName << ";";
        return { sqlBuf.str() };
    }

    SqlExpr getSqlRowsInsert(const bool insertValues = true) const
    {
        std::ostringstream sqlBuf, sqlBufValues;

        for (size_t rowIndex = 0; rowIndex < this->getMaxRowsCount(); ++rowIndex) {

            sqlBuf << "INSERT INTO " << this->tableName << " (";
            sqlBufValues << "VALUES (";

            for (auto& c: tableColumns)
                if (!c->is_AUTOINCREMENT()) {
                    sqlBuf << std::endl << c->getSqlName() << ",";
                    sqlBufValues << std::endl;
                    if (insertValues) sqlBufValues << c->getSqlRowValue(rowIndex);
                    else sqlBufValues << ":" << c->getSqlName('_') << '_' << rowIndex;
                    sqlBufValues << ",";
                }

            sqlBuf.seekp(-1, sqlBuf.cur);
            sqlBuf << std::endl << ")";

            sqlBufValues.seekp(-1, sqlBuf.cur);
            sqlBufValues << std::endl << ")";

            sqlBuf << std::endl << sqlBufValues.str() << ";" << std::endl;

            sqlBufValues.str("");
            sqlBufValues.clear();
        }

        sqlBuf.seekp(-1, sqlBuf.cur);
        sqlBuf << '\0';

        return { sqlBuf.str() };
    }

    SqlExpr getSqlRowsUpdate(
        const TableColumnBase::TableColumnComparsionExpr& condition   = {},
        const size_t& count  = 0,
        const size_t& offset = 0) const
    {
        std::ostringstream sqlBuf;

        for (size_t rowIndex = 0; rowIndex < this->getMaxRowsCount(); ++rowIndex) {

            sqlBuf << "UPDATE " << this->tableName << " SET";

            for (auto& c: tableColumns)
                if (!c->is_AUTOINCREMENT())
                    sqlBuf << std::endl << c->getSqlName() << "=" << c->getSqlRowValue(rowIndex) << ",";

            sqlBuf.seekp(-1, sqlBuf.cur);
            sqlBuf<<'\0';

            if (condition.expr != "")
                sqlBuf << std::endl << "WHERE " << condition;

            if (count > 0)
                sqlBuf << std::endl << "LIMIT " << count;

            if (offset > 0)
                sqlBuf << std::endl << "OFFSET " << offset;

            sqlBuf << ';' << std::endl;
        }

        sqlBuf.seekp(-1, sqlBuf.cur);
        sqlBuf << '\0';

        return { sqlBuf.str() };
    }

    SqlExpr getSqlRowsSelect(
        const std::vector<std::reference_wrapper<TableColumnBase>> sc = {},
        const TableColumnBase::TableColumnComparsionExpr& condition   = {},
        const std::vector<TableRowSort> sort = {},
        const size_t& count  = 0,
        const size_t& offset = 0) const
    {
        std::ostringstream sqlBuf;

        sqlBuf << "SELECT";

        if (sc.size() > 0) {
            for (auto& c: sc)
                sqlBuf << std::endl << c.get().getSqlName() << ",";
            sqlBuf.seekp(-1, sqlBuf.cur);
        }
        else sqlBuf << std::endl << this->tableName << ".*";

        sqlBuf << std::endl << "FROM " << this->tableName;

        if (condition.expr != "")
            sqlBuf << std::endl << "WHERE " << condition;

        if (sort.size() > 0) {
            sqlBuf << std::endl << "ORDER BY";
            for (auto& s: sort) {
                sqlBuf << " " << s.column.getSqlName();
                if (s.sortType != TableRowSort::TableRowSortType::DEF)
                    sqlBuf << " " << (s.sortType == TableRowSort::TableRowSortType::DESC ? "DESC" : "ASC");
                sqlBuf << ",";
            }
            sqlBuf.seekp(-1, sqlBuf.cur);
            sqlBuf << '\0';
        }

        if (count > 0)
            sqlBuf << std::endl << "LIMIT " << count;

        if (offset > 0)
            sqlBuf << std::endl << "OFFSET " << offset;

        sqlBuf << ';';

        return { sqlBuf.str() };
    }

    SqlExpr getSqlRowsDelete(
        const TableColumnBase::TableColumnComparsionExpr& condition   = {},
        const size_t& count  = 0,
        const size_t& offset = 0) const
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

    void clearRows()
    { for (auto& cl: this->tableColumns) cl->clearRows(); }

    size_t getMaxRowsCount() const
    { size_t rows = 0; for (auto& cl: this->tableColumns) if (rows < cl->getRowsCount()) rows = cl->getRowsCount(); return rows; }

    template<typename... T>
    void addRow(T&&... t)
    {
        assert(this->tableColumns.size() == sizeof...(T));

        size_t colBuf = 0;
        (void)std::initializer_list<int>{(addRow(colBuf++, std::forward<T>(t)), void(), 0)...};
    }

    template <typename T>
    void addRow(size_t colIndex, const T val)
    {
        assert(colIndex < this->tableColumns.size());

        auto buf = stdstr_or_type(val);

        assert(TableColumnBase::getTableColumnType<typeof(buf)>() == this->tableColumns[colIndex]->columnType);

        this->tableColumns[colIndex]->addRowPtr(std::make_shared<typeof(buf)>(buf));
    }
};

typedef Table::TableRowSort                                 TRS;
typedef Table::TableRowSort::TableRowSortType               TRST;

typedef Table::TableColumnBase::TableColumnType             TCT;
typedef Table::TableColumnBase::TableColumnSpecs            TCS;
typedef Table::TableColumnBase::TableColumnComparsionExpr   TCCE;

template<typename T>
struct TableColumn: Table::TableColumnBase
{
    static_assert(
        !( std::is_same<T, bool>::value
        && std::is_same<T, int>::value
        && std::is_same<T, float>::value
        && std::is_same<T, double>::value
        && std::is_same<T, std::time_t>::value
        && std::is_same<T, std::string>::value),
        "unknown data type"
    );

    std::vector<T> data;

    TableColumn (const SqlName& name = {}, TCS specs = TCS::EMPTY)
    {
        this->columnType  = getTableColumnType<T>();
        this->columnName  = name;
        this->columnSpecs = specs;
    }
    TableColumn (TCS specs): TableColumn({}, specs) {}
    TableColumn (const SqlName& name, const TCS specs, const T defValue): TableColumn(name, specs)  { this->data.push_back(defValue); }
    TableColumn (const SqlName& name, const T defValue): TableColumn(name)                          { this->data.push_back(defValue); }
    TableColumn (const TCS specs, const T defValue): TableColumn({}, specs)                         { this->data.push_back(defValue); }
    TableColumn (const T defValue): TableColumn()                                                   { this->data.push_back(defValue); }

    size_t getRowsCount() const override { return this->data.size(); }
    void clearRows() override { this->data.clear(); }

    void addRowPtr(const std::shared_ptr<void> rowPtr) override
    { if (rowPtr != nullptr) this->data.push_back(*(std::static_pointer_cast<T>(rowPtr))); }

    SqlExpr getSqlRowValue(size_t rowIndex = 0) const override
    {
        std::ostringstream sqlBuf;

        bool valueExists = false;

        if ((valueExists = rowIndex < getRowsCount())) {
            if (this->columnType == TCT::TEXT) sqlBuf << "'";
            sqlBuf << stringify(this->data[rowIndex]);
            if (this->columnType == TCT::TEXT) sqlBuf << "'";
        }

        if (!valueExists)
            if ((valueExists = this->is_NOT_NULL())) {
                switch (this->columnType) {
                    case TCT::INTEGER:  sqlBuf << "0";                  break;
                    case TCT::REAL:     sqlBuf << "0";                  break;
                    case TCT::DATETIME: sqlBuf << "CURRENT_TIMESTAMP";  break;
                    case TCT::TEXT:     sqlBuf << "''";                 break;
                    default: throw "UNKNOWN DATA TYPE";
                }
            }

        if (!valueExists) sqlBuf << "NULL";

        return { sqlBuf.str() };
    }

    SqlExpr getSqlCreateTable() const override
    {
        std::ostringstream sqlBuf;

        sqlBuf << this->columnName;

        sqlBuf << " ";
        switch (this->columnType) {
            case TCT::INTEGER:  sqlBuf << "INTEGER";  break;
            case TCT::REAL:     sqlBuf << "REAL";     break;
            case TCT::DATETIME: sqlBuf << "DATETIME"; break;
            case TCT::TEXT:     sqlBuf << "TEXT";     break;
            default: throw "UNKNOWN DATA TYPE";
        }

        if (this->is_PRIMARY_KEY())      sqlBuf << " " << "PRIMARY KEY";
        if (this->is_AUTOINCREMENT())    sqlBuf << " " << "AUTOINCREMENT";
        if (this->is_NOT_NULL())         sqlBuf << " " << "NOT NULL";
        if (this->is_DEFAULT())          sqlBuf << " " << "DEFAULT";

        if (this->is_NOT_NULL() || this->is_DEFAULT())
            sqlBuf << " " << this->getSqlRowValue();

        return { sqlBuf.str() };
    }

    inline TableColumnComparsionExpr operator>  (const SqlName& second)   const { return Table::TableColumnBase::operator>(second);  }
    inline TableColumnComparsionExpr operator<  (const SqlName& second)   const { return Table::TableColumnBase::operator<(second);  }
    inline TableColumnComparsionExpr operator== (const SqlName& second)   const { return Table::TableColumnBase::operator==(second); }
    inline TableColumnComparsionExpr operator!= (const SqlName& second)   const { return Table::TableColumnBase::operator!=(second); }
    inline TableColumnComparsionExpr IN (const SqlName& second)           const { return Table::TableColumnBase::IN(second); }

    inline TableColumnComparsionExpr operator>  (const TableColumnBase& second)   const { return Table::TableColumnBase::operator>(second);  }
    inline TableColumnComparsionExpr operator<  (const TableColumnBase& second)   const { return Table::TableColumnBase::operator<(second);  }
    inline TableColumnComparsionExpr operator== (const TableColumnBase& second)   const { return Table::TableColumnBase::operator==(second); }
    inline TableColumnComparsionExpr operator!= (const TableColumnBase& second)   const { return Table::TableColumnBase::operator!=(second); }
    inline TableColumnComparsionExpr IN (const TableColumnBase& second)           const { return Table::TableColumnBase::IN(second); }

    inline TableColumnComparsionExpr IN (const TableColumnComparsionExpr& second) const { return Table::TableColumnBase::IN(second); }

    TableColumnComparsionExpr operator> (const T& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) > static_cast<TableColumnComparsionExpr>(TableColumn<T>(second));  }

    inline TableColumnComparsionExpr operator< (const T& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) < static_cast<TableColumnComparsionExpr>(TableColumn<T>(second));  }

    inline TableColumnComparsionExpr operator== (const T& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) == static_cast<TableColumnComparsionExpr>(TableColumn<T>(second)); }

    inline TableColumnComparsionExpr operator!= (const T& second) const
    { return static_cast<TableColumnComparsionExpr>(*this) != static_cast<TableColumnComparsionExpr>(TableColumn<T>(second)); }
};

}

#endif // EORM_H
