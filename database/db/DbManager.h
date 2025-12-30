#pragma once
#include <QMutex>
#include <QSqlDatabase>
#include <QString>
#include <QThreadStorage>
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
    // 获取当前线程的数据库连接（线程安全）
    QSqlDatabase db() const;

private:
    DbManager() = default;
    // 为当前线程创建一个独立的数据库连接
    QSqlDatabase getOrCreateThreadConnection() const;

private:
    mutable QMutex mtx_;
    DbConfig cfg_;
    // 使用 QThreadStorage 为每个线程存储独立的数据库连接
    mutable QThreadStorage<QSqlDatabase> threadLocalDb_;
};

} // namespace db
