#include "DbManager.h"
#include <QSqlError>
#include <QStringList>
#include <QThread>
#include <QDebug>

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

    // 验证配置（不在这里创建连接，让每个线程创建自己的）
    const QString connStr = !cfg_.odbcConnStr.trimmed().isEmpty()
        ? cfg_.odbcConnStr.trimmed()
        : QStringLiteral("DSN=%1;").arg(cfg_.odbcDsn.trimmed());

    if (connStr.trimmed().isEmpty()) {
        return Result::fail(ErrorCode::InvalidParam, "Empty ODBC connection string/DSN.");
    }

    return Result::ok();
}

void DbManager::close() {
    QMutexLocker locker(&mtx_);
    // 清空线程本地存储中的所有连接
    threadLocalDb_.setLocalData(QSqlDatabase());
}

QSqlDatabase DbManager::db() const {
    // 获取或创建当前线程的数据库连接
    return getOrCreateThreadConnection();
}

QSqlDatabase DbManager::getOrCreateThreadConnection() const {
    // 检查当前线程是否已有连接
    QSqlDatabase threadDb = threadLocalDb_.localData();
    if (threadDb.isValid() && threadDb.isOpen()) {
        return threadDb;
    }

    // 当前线程没有连接，创建一个新的
    const QString driver = QStringLiteral("QODBC");
    QString connectionName = cfg_.connectionName + "_" + QString::number((qint64)QThread::currentThread());
    
    // 检查是否已经添加过该连接
    if (QSqlDatabase::contains(connectionName)) {
        threadDb = QSqlDatabase::database(connectionName);
        if (threadDb.isOpen()) {
            threadLocalDb_.setLocalData(threadDb);
            return threadDb;
        }
    } else {
        threadDb = QSqlDatabase::addDatabase(driver, connectionName);
    }

    const QString connStr = !cfg_.odbcConnStr.trimmed().isEmpty()
        ? cfg_.odbcConnStr.trimmed()
        : QStringLiteral("DSN=%1;").arg(cfg_.odbcDsn.trimmed());

    threadDb.setDatabaseName(connStr);

    if (!threadDb.open()) {
        qWarning() << "Failed to open database for thread" << QThread::currentThread() 
                   << "Error:" << threadDb.lastError().text();
        return QSqlDatabase();
    }

    threadLocalDb_.setLocalData(threadDb);
    return threadDb;
}

} // namespace db
