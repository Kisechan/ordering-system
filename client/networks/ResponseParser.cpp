#include "ResponseParser.h"

namespace ResponseParser {

    Response parseResponse(const QJsonObject& json) {
        Response response;

        // 检查必要字段
        if (!json.contains("code") || !json.contains("data")) {
            response.isValid = false;
            response.code = UNKNOWN_ERROR;
            response.message = "响应格式无效";
            return response;
        }

        // 提取响应码
        if (!json.value("code").isDouble()) {
            response.isValid = false;
            response.code = UNKNOWN_ERROR;
            response.message = "响应码格式无效";
            return response;
        }
        response.code = json.value("code").toInt();

        // 提取消息
        if (json.contains("msg") && json.value("msg").isString()) {
            response.message = json.value("msg").toString();
        }

        // 提取数据
        if (json.value("data").isObject()) {
            response.data = json.value("data").toObject();
        } else if (json.value("data").isArray()) {
            // 如果是数组，包装为对象
            QJsonObject dataWrapper;
            dataWrapper["array"] = json.value("data").toArray();
            response.data = dataWrapper;
        }

        response.isValid = true;
        return response;
    }

    bool isSuccess(const Response& response) {
        return response.isValid && response.code == SUCCESS;
    }

    QString getErrorDescription(int code) {
        switch (code) {
        case SUCCESS:
            return "成功";
        case PASSWORD_ERROR:
            return "密码错误";
        case USER_NOT_FOUND:
            return "用户不存在";
        case INVALID_PASSWORD_FORMAT:
            return "密码格式不合法，只能包含字母和数字";
        case UNKNOWN_ERROR:
        default:
            return "未知错误";
        }
    }

} // namespace ResponseParser
