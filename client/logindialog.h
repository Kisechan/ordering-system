#pragma once

#include <QDialog>
#include <QString>

class ElaLineEdit;
class ElaCheckBox;
class ElaPushButton;
class ElaText;
class NetworkManager;

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(NetworkManager* networkMgr, QWidget* parent = nullptr);

    QString username() const;
    QString password() const;
    int getUserId() const;
    QString getUsername() const;
    bool rememberMe() const;
    bool autoLogin() const;

signals:
    void loginSubmitted(const QString& user,
                        const QString& pass,
                        bool remember,
                        bool autoLogin);

private slots:
    void onLoginClicked();
    void onCancelClicked();
    void onRegisterLinkActivated(const QString& link);
    
    // NetworkManager 响应槽
    void onLoginSuccess();
    void onLoginFailed(const QString& error);
    void onRegisterSuccess();
    void onRegisterFailed(const QString& error);

private:
    void setErrorText(const QString& msg);

private:
    ElaLineEdit*   m_userEdit   = nullptr;
    ElaLineEdit*   m_passEdit   = nullptr;
    ElaCheckBox*   m_remember   = nullptr;
    ElaCheckBox*   m_autoLogin  = nullptr;

    ElaPushButton* m_loginBtn   = nullptr;
    ElaPushButton* m_cancelBtn  = nullptr;
    ElaText*       m_errorText  = nullptr;
    
    NetworkManager* m_networkMgr = nullptr;

private:
    QString s_username;
    int s_userId;
};
