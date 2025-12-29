#pragma once
#include <QString>

namespace db {

enum class ErrorCode {
    Ok = 0,
    DriverMissing,
    DbNotOpen,
    InvalidParam
};

struct Result {
    ErrorCode code = ErrorCode::Ok;
    QString message;

    bool isOk() const { return code == ErrorCode::Ok; }

    static Result ok() { return {ErrorCode::Ok, ""}; }
    static Result fail(ErrorCode c, const QString& msg) { return {c, msg}; }
};

} // namespace db
