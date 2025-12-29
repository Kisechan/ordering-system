#include <QApplication>
#include "ElaApplication.h"
#include "logindialog.h"
#include "clientmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    eApp->init();

    QFont f = qApp->font();
    f.setPixelSize(17);
    qApp->setFont(f);

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        auto* w = new ClientMainWindow();
        w->show();
        return a.exec();
    }

    return 0;
}
