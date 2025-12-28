#pragma once
#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>
#include <QSqlQuery>
#include <QString>

namespace db {

inline bool dbIsOpen(const QSqlDatabase& db, QString* err = nullptr) {
    if (!db.isValid()) {
        if (err) *err = "Invalid database handle.";
        return false;
    }
    if (!db.isOpen()) {
        if (err) *err = "Database is not open.";
        return false;
    }
    return true;
}

inline QString sqlErr(const QSqlQuery& q) { return q.lastError().text(); }

inline bool isAlphaNumOnly(const QString& s) {
    for (const auto& ch : s) {
        if (!ch.isLetterOrNumber()) return false;
    }
    return true;
}

} // namespace db
