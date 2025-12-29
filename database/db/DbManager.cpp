#include "DbManager.h"
#include <QSqlError>
#include <QStringList>

namespace db {

DbManager& DbManager::instance() {
    static DbManager inst;
    return inst;
}

Result DbManager::init(const DbConfig& cfg) {
    QMutexLocker locker(&mtx_);
    cfg_ = cfg;

    const QString driver = QStringLiteral("QODBC");
    if (!QSqlDatabase::drivers().contains(driver)) {
        return Result::fail(ErrorCode::DriverMissing, "QODBC driver missing (qsqlodbc plugin).");
    }

    if (QSqlDatabase::contains(cfg_.connectionName)) {
        db_ = QSqlDatabase::database(cfg_.connectionName);
        if (db_.isOpen()) db_.close();
    } else {
        db_ = QSqlDatabase::addDatabase(driver, cfg_.connectionName);
    }

    const QString connStr = !cfg_.odbcConnStr.trimmed().isEmpty()
        ? cfg_.odbcConnStr.trimmed()
        : QStringLiteral("DSN=%1;").arg(cfg_.odbcDsn.trimmed());

    if (connStr.trimmed().isEmpty()) {
        return Result::fail(ErrorCode::InvalidParam, "Empty ODBC connection string/DSN.");
    }

    db_.setDatabaseName(connStr);

    if (!db_.open()) {
        return Result::fail(ErrorCode::DbNotOpen, db_.lastError().text());
    }
    return Result::ok();
}

void DbManager::close() {
    QMutexLocker locker(&mtx_);
    if (db_.isValid() && db_.isOpen()) db_.close();
}

QSqlDatabase DbManager::db() const {
    QMutexLocker locker(&mtx_);
    return db_;
}

} // namespace db
