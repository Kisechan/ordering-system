#include "logindialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QImage>
#include <QLabel>
#include <QEvent>

#include "ElaLineEdit.h"
#include "ElaCheckBox.h"
#include "ElaPushButton.h"
#include "ElaText.h"
#include "ElaImageCard.h"

// 需要你已把 RegisterDialog 加入工程
#include "registerdialog.h"

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(QStringLiteral("登录"));
    setModal(true);
    setFixedSize(420, 520);
    QFont f;

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(14);

    // 顶部图片（ElaImageCard）
    auto* logo = new ElaImageCard(this);
    logo->setFixedSize(88, 88);
    logo->setBorderRadius(12);
    logo->setIsPreserveAspectCrop(true);

    QImage img(QStringLiteral(":/include/Image/Moon.jpg"));
    if (!img.isNull())
        logo->setCardImage(img);

    root->addWidget(logo, 0, Qt::AlignHCenter);

    auto* title = new ElaText(QStringLiteral("自助点餐系统"), 22, this);
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    auto* sub = new ElaText(QStringLiteral("请使用账号密码登录"), 15, this);
    sub->setAlignment(Qt::AlignCenter);
    root->addWidget(sub);

    root->addSpacing(10);

    // 账号
    auto* userLabel = new ElaText(QStringLiteral("账号"), 13, this);
    root->addWidget(userLabel);


    m_userEdit = new ElaLineEdit(this);
    m_userEdit->setPlaceholderText(QStringLiteral("请输入账号 / 邮箱 / 手机号"));
    m_userEdit->setClearButtonEnabled(true);
    m_userEdit->setMinimumHeight(38);
    f = m_userEdit->font();
    f.setPixelSize(13);
    m_userEdit->setFont(f);
    root->addWidget(m_userEdit);

    // 密码
    auto* passLabel = new ElaText(QStringLiteral("密码"), 13, this);
    root->addWidget(passLabel);

    m_passEdit = new ElaLineEdit(this);
    m_passEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setClearButtonEnabled(true);
    m_passEdit->setMinimumHeight(38);
    f = m_passEdit->font();
    f.setPixelSize(13);
    m_passEdit->setFont(f);
    root->addWidget(m_passEdit);

    // 选项 + 注册链接
    auto* optRow = new QHBoxLayout();
    optRow->setContentsMargins(0, 6, 0, 0);
    m_remember  = new ElaCheckBox(QStringLiteral("记住我"), this);
    m_autoLogin = new ElaCheckBox(QStringLiteral("自动登录"), this);

    // 原生 QLabel 超链接 + 悬停改色（Enter/Leave 稳定回退）
    auto makeLinkHtml = [](const QString& color) {
        return QStringLiteral(
            "<a href=\"register\">"
            "<span style=\"color:%1; text-decoration:none;\">没有账号？注册</span>"
            "</a>"
        ).arg(color);
    };

    auto* registerLink = new QLabel(this);
    registerLink->setText(makeLinkHtml("#2d7dff"));  // 默认蓝色
    registerLink->setTextFormat(Qt::RichText);
    registerLink->setTextInteractionFlags(Qt::TextBrowserInteraction);
    registerLink->setOpenExternalLinks(false);
    registerLink->setCursor(Qt::PointingHandCursor);
    registerLink->setStyleSheet("QLabel { background: transparent; }");
    registerLink->setAttribute(Qt::WA_Hover, true);
    f = registerLink->font();
    f.setPixelSize(13);
    registerLink->setFont(f);

    // eventFilter：只作用在这个 QLabel，不用改别处
    class _LinkHoverFilter final : public QObject {
    public:
        _LinkHoverFilter(QLabel* w, decltype(makeLinkHtml) f)
            : QObject(w), w_(w), make_(f) {}

    protected:
        bool eventFilter(QObject* obj, QEvent* ev) override
        {
            if (obj == w_) {
                if (ev->type() == QEvent::Enter) {
                    w_->setText(make_("#8B5A2B")); // 悬停棕色
                } else if (ev->type() == QEvent::Leave) {
                    w_->setText(make_("#2d7dff")); // 离开恢复蓝色
                }
            }
            return QObject::eventFilter(obj, ev);
        }

    private:
        QLabel* w_;
        decltype(makeLinkHtml) make_;
    };

    registerLink->installEventFilter(new _LinkHoverFilter(registerLink, makeLinkHtml));

    connect(registerLink, &QLabel::linkActivated, this, &LoginDialog::onRegisterLinkActivated);

    optRow->addWidget(m_remember);
    optRow->addSpacing(16);
    optRow->addWidget(m_autoLogin);
    optRow->addSpacing(12);
    optRow->addWidget(registerLink);
    optRow->addStretch(1);

    root->addLayout(optRow);

    // 错误提示
    m_errorText = new ElaText("", 12, this);
    m_errorText->setStyleSheet("color: #ff5c5c;");
    root->addWidget(m_errorText);

    root->addStretch(1);

    // 按钮区
    auto* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    m_cancelBtn = new ElaPushButton(QStringLiteral("取消"), this);
    m_cancelBtn->setMinimumHeight(40);

    m_loginBtn = new ElaPushButton(QStringLiteral("登录"), this);
    m_loginBtn->setMinimumHeight(40);

    btnRow->addWidget(m_cancelBtn);
    btnRow->addWidget(m_loginBtn);

    root->addLayout(btnRow);

    // 连接
    connect(m_loginBtn,  &ElaPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_cancelBtn, &ElaPushButton::clicked, this, &LoginDialog::onCancelClicked);

    connect(m_userEdit, &ElaLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(m_passEdit, &ElaLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);

    m_userEdit->setFocus();
}

QString LoginDialog::username() const
{
    return m_userEdit ? m_userEdit->text() : QString();
}

QString LoginDialog::password() const
{
    return m_passEdit ? m_passEdit->text() : QString();
}

bool LoginDialog::rememberMe() const
{
    return m_remember && m_remember->isChecked();
}

bool LoginDialog::autoLogin() const
{
    return m_autoLogin && m_autoLogin->isChecked();
}

void LoginDialog::setErrorText(const QString& msg)
{
    if (m_errorText) m_errorText->setText(msg);
}

void LoginDialog::onLoginClicked()
{
    const QString u = username().trimmed();
    const QString p = password();

    if (u.isEmpty()) { setErrorText(QStringLiteral("账号不能为空")); return; }
    if (p.isEmpty()) { setErrorText(QStringLiteral("密码不能为空")); return; }

    setErrorText(QString());

    // 这里需要实现登录判定
    // .......................................................................
    accept(); // accpet() 函数表示成功
}

void LoginDialog::onCancelClicked()
{
    reject();
}

void LoginDialog::onRegisterLinkActivated(const QString& /*link*/)
{
    RegisterDialog dlg(this);

    // 示例：把 RegisterDialog 的注册请求接到你的后端/数据库逻辑
    connect(&dlg, &RegisterDialog::registerSubmitted, this,
            [&dlg](const QString& u, const QString& p) {
                // 例：用户名 admin 已存在
                if (u == QStringLiteral("admin")) {
                    dlg.applyRegisterResult(RegisterDialog::RegisterResult::UserAlreadyExists,
                                            QStringLiteral("该账号已存在，请更换"));
                    return;
                }
                // 正常成功
                dlg.applyRegisterResult(RegisterDialog::RegisterResult::Success,
                                        QStringLiteral("注册成功，请返回登录"));
            });

    if (dlg.exec() == QDialog::Accepted) {
        m_userEdit->setText(dlg.username());
        m_passEdit->clear();
        m_passEdit->setFocus();
    }
}
