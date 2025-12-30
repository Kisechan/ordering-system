#include "accountpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>

#include "ElaScrollArea.h"
#include "ElaText.h"
#include "ElaLineEdit.h"
#include "ElaPushButton.h"

AccountPage::AccountPage(int userId, const QString& username, QWidget* parent)
    : ElaScrollPage(parent), m_userId(userId)
{
    setTitleVisible(false);
    setPageTitleSpacing(0);

    m_content = new QWidget(this);
    m_root = new QVBoxLayout(m_content);
    m_root->setContentsMargins(20, 18, 20, 18);
    m_root->setSpacing(14);

    // ===================== 1) 修改用户名（框） =====================
    {
        QWidget* box = makeSection(QStringLiteral("用户名修改"));
        auto* lay = static_cast<QVBoxLayout*>(box->layout());

        m_currentNameText = new ElaText(QStringLiteral("当前用户名：%1").arg(username), 15, box);
        m_currentNameText->setStyleSheet("color:#666666;");
        lay->addWidget(m_currentNameText);

        auto* row = new QHBoxLayout();
        row->setContentsMargins(0, 8, 0, 0);
        row->setSpacing(10);

        m_newNameEdit = new ElaLineEdit(box);
        m_newNameEdit->setPlaceholderText(QStringLiteral("输入新用户名"));
        m_newNameEdit->setMinimumHeight(40);
        row->addWidget(m_newNameEdit, 1);

        m_saveNameBtn = new ElaPushButton(QStringLiteral("保存"), box);
        m_saveNameBtn->setMinimumHeight(40);
        row->addWidget(m_saveNameBtn, 0);

        lay->addLayout(row);

        connect(m_saveNameBtn, &ElaPushButton::clicked, this, [this] { onSaveUsername(); });
        connect(m_newNameEdit, &ElaLineEdit::returnPressed, this, [this] { onSaveUsername(); });

        m_root->addWidget(box);
    }

    // ===================== 2) 修改密码（框） =====================
    {
        QWidget* box = makeSection(QStringLiteral("密码修改"));
        auto* lay = static_cast<QVBoxLayout*>(box->layout());

        m_oldPassEdit = new ElaLineEdit(box);
        m_oldPassEdit->setPlaceholderText(QStringLiteral("旧密码"));
        m_oldPassEdit->setEchoMode(QLineEdit::Password);
        m_oldPassEdit->setMinimumHeight(40);
        lay->addWidget(m_oldPassEdit);

        m_newPassEdit = new ElaLineEdit(box);
        m_newPassEdit->setPlaceholderText(QStringLiteral("新密码"));
        m_newPassEdit->setEchoMode(QLineEdit::Password);
        m_newPassEdit->setMinimumHeight(40);
        lay->addWidget(m_newPassEdit);

        m_newPass2Edit = new ElaLineEdit(box);
        m_newPass2Edit->setPlaceholderText(QStringLiteral("确认新密码"));
        m_newPass2Edit->setEchoMode(QLineEdit::Password);
        m_newPass2Edit->setMinimumHeight(40);
        lay->addWidget(m_newPass2Edit);

        auto* row = new QHBoxLayout();
        row->setContentsMargins(0, 8, 0, 0);
        row->addStretch(1);

        m_savePassBtn = new ElaPushButton(QStringLiteral("保存"), box);
        m_savePassBtn->setMinimumHeight(40);
        row->addWidget(m_savePassBtn);

        lay->addLayout(row);

        connect(m_savePassBtn, &ElaPushButton::clicked, this, [this] { onSavePassword(); });
        connect(m_newPass2Edit, &ElaLineEdit::returnPressed, this, [this] { onSavePassword(); });

        m_root->addWidget(box);
    }

    // ===================== 3) 退出登录（按钮） =====================
    {
        auto* row = new QHBoxLayout();
        row->setContentsMargins(0, 6, 0, 0);
        row->addStretch(1);

        m_logoutBtn = new ElaPushButton(QStringLiteral("退出登录"), m_content);
        m_logoutBtn->setMinimumHeight(42);
        m_logoutBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        row->addWidget(m_logoutBtn);

        row->addStretch(1);
        m_root->addLayout(row);

        connect(m_logoutBtn, &ElaPushButton::clicked, this, [this] { onLogout(); });
    }

    m_root->addStretch(1);

    addCentralWidget(m_content, true, true);
    if (auto* sa = this->findChild<ElaScrollArea*>()) {
        sa->setAlignment(Qt::AlignTop);
    }
}

QWidget* AccountPage::makeSection(const QString& title)
{
    auto* box = new QWidget(m_content);
    box->setAttribute(Qt::WA_StyledBackground, true);
    box->setObjectName("AccountSection");
    box->setStyleSheet(
        "#AccountSection{"
        "background-color:#f7f7f7;"
        "border:1px solid #d0d0d0;"
        "border-radius:12px;"
        "}"
    );

    auto* lay = new QVBoxLayout(box);
    lay->setContentsMargins(16, 14, 16, 14);
    lay->setSpacing(10);

    auto* t = new ElaText(title, 18, box);
    t->setStyleSheet("color:#222222;");
    lay->addWidget(t);

    return box;
}

void AccountPage::setCurrentUsername(const QString& username)
{
    if (m_currentNameText) {
        m_currentNameText->setText(QStringLiteral("当前用户名：%1").arg(username));
    }
}

void AccountPage::onSaveUsername()
{
    const QString newName = m_newNameEdit ? m_newNameEdit->text().trimmed() : QString();
    if (newName.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("新用户名不能为空"));
        return;
    }
    emit usernameUpdateRequested(m_userId, newName);
}

void AccountPage::onSavePassword()
{
    const QString oldP = m_oldPassEdit ? m_oldPassEdit->text() : QString();
    const QString newP = m_newPassEdit ? m_newPassEdit->text() : QString();
    const QString newP2 = m_newPass2Edit ? m_newPass2Edit->text() : QString();

    if (oldP.isEmpty() || newP.isEmpty() || newP2.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("密码项不能为空"));
        return;
    }
    if (newP != newP2) {
        QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("两次输入的新密码不一致"));
        return;
    }
    emit passwordUpdateRequested(m_userId, oldP, newP);
}

void AccountPage::onLogout()
{
    const auto ret = QMessageBox::question(this,
                                          QStringLiteral("确认"),
                                          QStringLiteral("确定要退出登录吗？"));
    if (ret == QMessageBox::Yes) {
        emit logoutRequested();
    }
}
