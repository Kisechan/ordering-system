#pragma once

#include <QDialog>
#include <QRegularExpression>

class ElaLineEdit;
class ElaPushButton;
class ElaText;
class ElaMessageBar;
class NetworkManager;

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    enum class RegisterResult {
        Success,
        UserAlreadyExists,
        InvalidPasswordFormat,
        PasswordMismatch,
        EmptyFields,
        UnknownError
    };
    Q_ENUM(RegisterResult)

    explicit RegisterDialog(NetworkManager* networkMgr, QWidget* parent = nullptr);

    QString username() const;
    QString password() const;

    // 外部处理注册请求后，用这个回填结果（成功/失败原因等）
    void applyRegisterResult(RegisterResult r, const QString& message = QString());

signals:
    // 注册请求：外部可连接到你的后端/数据库逻辑
    void registerSubmitted(const QString& username, const QString& password);

private slots:
    void onRegisterClicked();
    void onBackClicked();
    
    // NetworkManager 响应槽
    void onRegisterSuccess();
    void onRegisterFailed(const QString& error);

private:
    bool validateInputs(QString& err) const;
    bool isPasswordAlnumOnly(const QString& p) const;

private:
    ElaLineEdit*   m_userEdit   = nullptr;
    ElaLineEdit*   m_passEdit   = nullptr;
    ElaLineEdit*   m_pass2Edit  = nullptr;

    ElaText*       m_errorText  = nullptr;

    ElaPushButton* m_backBtn    = nullptr;
    ElaPushButton* m_registerBtn= nullptr;

    QRegularExpression m_passRe; // ^[A-Za-z0-9]+$
    
    NetworkManager* m_networkMgr = nullptr;
};
