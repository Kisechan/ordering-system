#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "server.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    Server* server = Server::getInstance();
    if (server && server->isListening()) {
        server->close();
        qDebug() << "Server closed";
    }
    event->accept();
}
