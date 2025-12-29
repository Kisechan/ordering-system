#pragma once
#include <QMutex>
#include <QSqlDatabase>
#include <QString>
#include "Result.h"

namespace db {

struct DbConfig {
    QString odbcConnStr;   // preferred: full ODBC conn string
    QString odbcDsn;       // optional DSN
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
