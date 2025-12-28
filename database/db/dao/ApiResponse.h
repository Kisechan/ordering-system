#pragma once
#include <QString>

namespace db {

// Unified response format in design.md:
// { "code": xxx, "msg": "...", "data": ... }
template <typename T>
struct ApiResponse {
    int code = 500;
    QString msg;
    T data{};

    bool ok() const { return code == 200; }
};

struct ApiResponseVoid {
    int code = 500;
    QString msg;

    bool ok() const { return code == 200; }
};

} // namespace db
