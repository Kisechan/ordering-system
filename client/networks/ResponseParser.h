#ifndef RESPONSEPARSER_H
#define RESPONSEPARSER_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>

namespace ResponseParser {

    // 响应码枚举
    enum ResponseCode {
        SUCCESS = 200,              // 请求成功
        PASSWORD_ERROR = 301,       // 密码错误
        USER_NOT_FOUND = 302,       // 用户不存在
        INVALID_PASSWORD_FORMAT = 303, // 密码格式不合法
        UNKNOWN_ERROR = 999         // 未知错误
    };

    // 响应结构体
    struct Response {
        int code;              // 响应码
        QString message;       // 响应消息
        QJsonObject data;      // 响应数据
        bool isValid;          // 响应是否有效

        Response() : code(UNKNOWN_ERROR), isValid(false) {}
    };

    // 解析服务器响应 JSON
    Response parseResponse(const QJsonObject& json);

    // 判断是否成功
    bool isSuccess(const Response& response);

    // 获取错误描述
    QString getErrorDescription(int code);

} // namespace ResponseParser

#endif // RESPONSEPARSER_H
