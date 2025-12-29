#pragma once
#include <QString>
#include <QDateTime>

namespace db {

class User {
public:
    int userId() const { return userId_; }
    void setUserId(int v) { userId_ = v; }

    const QString& username() const { return username_; }
    void setUsername(const QString& v) { username_ = v; }

    const QString& password() const { return password_; }
    void setPassword(const QString& v) { password_ = v; }

    const QDateTime& registerTime() const { return registerTime_; }
    void setRegisterTime(const QDateTime& v) { registerTime_ = v; }

private:
    int userId_ = 0;
    QString username_;
    QString password_;
    QDateTime registerTime_;
};

} // namespace db
