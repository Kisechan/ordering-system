#pragma once
#include <QSqlDatabase>
#include <QMutex>
#include <QString>

#include "Result.h"

namespace db {

struct DbConfig {
    // For ODBC, use a full connection string (recommended on Windows):
    // DRIVER={MariaDB ODBC 3.2 Driver};TCPIP=1;SERVER=localhost;PORT=3306;DATABASE=restaurant;UID=root;PWD=123456;OPTION=3;
    QString odbcConnStr;

    // Optional DSN path, not recommended for MariaDB ODBC on some Qt setups.
    QString odbcDsn;

    QString connectionName = "restaurant_conn";
};

class DbManager {
public:
    static DbManager& instance();

    Result init(const DbConfig& cfg);
    void close();

    QSqlDatabase db() const;

private:
    DbManager() = default;

private:
    mutable QMutex mtx_;
    DbConfig cfg_;
    QSqlDatabase db_;
};

} // namespace db
