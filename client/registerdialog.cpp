#include "registerdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>

#include "ElaLineEdit.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaContentDialog.h"

RegisterDialog::RegisterDialog(QWidget* parent)
    : QDialog(parent),
      m_passRe(QRegularExpression(QStringLiteral("^[A-Za-z0-9]+$")))
{
    setWindowTitle(QStringLiteral("注册账号"));
    setModal(true);
    setFixedSize(420, 520);
    QFont f;

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(14);

    auto* title = new ElaText(QStringLiteral("账号注册"), 22, this);
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    auto* sub = new ElaText(QStringLiteral("请填写账号与密码（仅数字/字母）"), 15, this);
    sub->setAlignment(Qt::AlignCenter);
    root->addWidget(sub);

    root->addSpacing(10);

    // 账号
    root->addWidget(new ElaText(QStringLiteral("账号"), 13, this));
    m_userEdit = new ElaLineEdit(this);
    m_userEdit->setPlaceholderText(QStringLiteral("请输入账号 / 邮箱 / 手机号"));
    m_userEdit->setClearButtonEnabled(true);
    m_userEdit->setMinimumHeight(38);
    f = m_userEdit->font();
    f.setPixelSize(13);
    m_userEdit->setFont(f);
    root->addWidget(m_userEdit);

    // 密码
    root->addWidget(new ElaText(QStringLiteral("密码"), 13, this));
    m_passEdit = new ElaLineEdit(this);
    m_passEdit->setPlaceholderText(QStringLiteral("请输入密码（仅数字/字母）"));
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setClearButtonEnabled(true);
    m_passEdit->setMinimumHeight(38);
    f = m_passEdit->font();
    f.setPixelSize(13);
    m_passEdit->setFont(f);
    root->addWidget(m_passEdit);

    // 确认密码
    root->addWidget(new ElaText(QStringLiteral("确认密码"), 13, this));
    m_pass2Edit = new ElaLineEdit(this);
    m_pass2Edit->setPlaceholderText(QStringLiteral("请再次输入密码"));
    m_pass2Edit->setEchoMode(QLineEdit::Password);
    m_pass2Edit->setClearButtonEnabled(true);
    m_pass2Edit->setMinimumHeight(38);
    f = m_pass2Edit->font();
    f.setPixelSize(13);
    m_pass2Edit->setFont(f);
    root->addWidget(m_pass2Edit);

    // 错误/提示
    m_errorText = new ElaText("", 12, this);
    m_errorText->setStyleSheet("color: #ff5c5c;"); // 默认红色
    root->addWidget(m_errorText);

    root->addStretch(1);

    // 按钮区：返回 / 注册
    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    m_backBtn = new ElaPushButton(QStringLiteral("返回"), this);
    m_backBtn->setMinimumHeight(40);

    m_registerBtn = new ElaPushButton(QStringLiteral("注册"), this);
    m_registerBtn->setMinimumHeight(40);

    btnRow->addWidget(m_backBtn);
    btnRow->addWidget(m_registerBtn);
    root->addLayout(btnRow);

    // 连接
    connect(m_backBtn, &QPushButton::clicked, this, &RegisterDialog::onBackClicked);
    connect(m_registerBtn, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);

    // 回车注册
    connect(m_userEdit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);
    connect(m_passEdit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);
    connect(m_pass2Edit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);

    m_userEdit->setFocus();
}

QString RegisterDialog::username() const
{
    return m_userEdit ? m_userEdit->text() : QString();
}

QString RegisterDialog::password() const
{
    return m_passEdit ? m_passEdit->text() : QString();
}

bool RegisterDialog::isPasswordAlnumOnly(const QString& p) const
{
    return m_passRe.match(p).hasMatch();
}

bool RegisterDialog::validateInputs(QString& err) const
{
    const QString u = username().trimmed();
    const QString p1 = password();
    const QString p2 = m_pass2Edit ? m_pass2Edit->text() : QString();

    if (u.isEmpty()) { err = QStringLiteral("账号不能为空"); return false; }
    if (p1.isEmpty() || p2.isEmpty()) { err = QStringLiteral("密码不能为空"); return false; }
    if (!isPasswordAlnumOnly(p1)) { err = QStringLiteral("密码格式错误：只能包含数字和字母"); return false; }
    if (p1 != p2) { err = QStringLiteral("两次输入的密码不一致"); return false; }

    return true;
}

void RegisterDialog::onRegisterClicked()
{
    QString err;
    if (!validateInputs(err)) {
        applyRegisterResult(RegisterResult::InvalidPasswordFormat, err);
        if (err == QStringLiteral("账号不能为空") || err == QStringLiteral("密码不能为空"))
            applyRegisterResult(RegisterResult::EmptyFields, err);
        else if (err == QStringLiteral("两次输入的密码不一致"))
            applyRegisterResult(RegisterResult::PasswordMismatch, err);
        else
            applyRegisterResult(RegisterResult::InvalidPasswordFormat, err);
        return;
    }

    // UI 层：发起注册请求（外部去真正写入数据库/发 HTTP 等）
    m_errorText->setStyleSheet("color:#888888;");
    m_errorText->setText(QStringLiteral("正在注册..."));

    // 这里需要补充注册逻辑，根据结果执行对应的 applyRegisterResult
    // ...........................................................

    applyRegisterResult(RegisterResult::Success, "注册成功");
}

void RegisterDialog::applyRegisterResult(RegisterResult r, const QString& message)
{
    if (!m_errorText) return;

    switch (r) {
    case RegisterResult::Success: {
        m_errorText->setStyleSheet("color: #2aa84a;");
        m_errorText->setText(message.isEmpty() ? QStringLiteral("注册成功") : message);

        QMessageBox::information(this, QStringLiteral("提示"), message);
        accept();
        break;
    }

    case RegisterResult::UserAlreadyExists:
        m_errorText->setStyleSheet("color: #ff5c5c;");
        m_errorText->setText(message.isEmpty() ? QStringLiteral("用户已存在") : message);
        break;

    case RegisterResult::InvalidPasswordFormat:
        m_errorText->setStyleSheet("color: #ff5c5c;");
        m_errorText->setText(message.isEmpty() ? QStringLiteral("密码格式错误：只能包含数字和字母") : message);
        break;

    case RegisterResult::PasswordMismatch:
        m_errorText->setStyleSheet("color: #ff5c5c;");
        m_errorText->setText(message.isEmpty() ? QStringLiteral("两次输入的密码不一致") : message);
        break;

    case RegisterResult::EmptyFields:
        m_errorText->setStyleSheet("color: #ff5c5c;");
        m_errorText->setText(message.isEmpty() ? QStringLiteral("请完整填写信息") : message);
        break;

    default:
        m_errorText->setStyleSheet("color: #ff5c5c;");
        m_errorText->setText(message.isEmpty() ? QStringLiteral("注册失败，请稍后重试") : message);
        break;
    }
}

void RegisterDialog::onBackClicked()
{
    reject();
}
