#pragma once
#include <QDateTime>
#include <QString>

namespace db {

class AdminOrderBrief {
public:
    int orderId() const { return orderId_; }
    void setOrderId(int v) { orderId_ = v; }

    const QString& username() const { return username_; }
    void setUsername(const QString& v) { username_ = v; }

    double totalAmount() const { return totalAmount_; }
    void setTotalAmount(double v) { totalAmount_ = v; }

    const QDateTime& createTime() const { return createTime_; }
    void setCreateTime(const QDateTime& v) { createTime_ = v; }

    const QString& comment() const { return comment_; }
    void setComment(const QString& v) { comment_ = v; }

private:
    int orderId_ = 0;
    QString username_;
    double totalAmount_ = 0.0;
    QDateTime createTime_;
    QString comment_;
};

} // namespace db
