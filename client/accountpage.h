#pragma once
#include "ElaScrollPage.h"

#include <QString>

class QWidget;
class QVBoxLayout;
class ElaLineEdit;
class ElaText;
class ElaPushButton;

class AccountPage : public ElaScrollPage
{
    Q_OBJECT
public:
    explicit AccountPage(int userId, const QString& username, QWidget* parent = nullptr);

    // 让外部（比如 ClientMainWindow）在用户名改完后更新 UI
    void setCurrentUsername(const QString& username);

signals:
    // 预留后端接口：改用户名 / 改密码 / 退出登录
    void usernameUpdateRequested(int userId, const QString& newUsername);
    void passwordUpdateRequested(int userId, const QString& oldPass, const QString& newPass);
    void logoutRequested();

private:
    QWidget* makeSection(const QString& title);
    void onSaveUsername();
    void onSavePassword();
    void onLogout();

private:
    int m_userId = 0;

    QWidget* m_content = nullptr;
    QVBoxLayout* m_root = nullptr;

    // 修改用户名
    ElaText* m_currentNameText = nullptr;
    ElaLineEdit* m_newNameEdit = nullptr;
    ElaPushButton* m_saveNameBtn = nullptr;

    // 修改密码
    ElaLineEdit* m_oldPassEdit = nullptr;
    ElaLineEdit* m_newPassEdit = nullptr;
    ElaLineEdit* m_newPass2Edit = nullptr;
    ElaPushButton* m_savePassBtn = nullptr;

    // 退出登录
    ElaPushButton* m_logoutBtn = nullptr;
};
