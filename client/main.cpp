#include "mainwindow.h"
#include <QApplication>
#include "ElaApplication.h"
#include "ElaAppBar.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
