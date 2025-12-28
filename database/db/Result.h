#pragma once
#include <QString>

namespace db {

enum class ErrorCode {
    Ok = 0,
    DriverMissing = 1,
    DbNotOpen = 2,
    SqlError = 3,
    InvalidParam = 4,
    NotFound = 5,
    Conflict = 6
};

struct Result {
    ErrorCode code = ErrorCode::Ok;
    QString message;

    bool isOk() const { return code == ErrorCode::Ok; }

    static Result ok() { return {ErrorCode::Ok, ""}; }
    static Result fail(ErrorCode c, const QString& msg) { return {c, msg}; }
};

} // namespace db
