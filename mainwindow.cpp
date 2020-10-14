#include "mainwindow.h"
#include "ui_mainwindow.h"

#define WRITE_ERROR -1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    msgRef = 0;

    socket_1 = new QTcpSocket();
    socket_2 = new QTcpSocket();
    socket_3 = new QTcpSocket();
}

MainWindow::~MainWindow() {
    delete ui;
}

bool MainWindow::archonConnect(QTcpSocket *socket, QString ipAddr, QString portNumber) {
    if (ipAddr == "" && portNumber == "") { return false; }

    socket->connectToHost(QHostAddress(ipAddr), portNumber.toUShort());

    return socket->waitForConnected();
}

bool MainWindow::archonDisconnect(QTcpSocket *socket) {
    socket->disconnectFromHost();

    return socket->waitForDisconnected();
}

int MainWindow::archonSend(QTcpSocket *socket, QString preCommand) {
    QString postCommand;

    postCommand.sprintf(">%02X%s\n", msgRef, preCommand.toStdString().c_str());

    return socket->write(postCommand.toStdString().c_str(), postCommand.size());
}

