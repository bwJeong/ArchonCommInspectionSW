#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QVector>
#include <QMap>
#include <QHash>
#include <QQueue>
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    bool archonConnect(QTcpSocket *socket, QString ipAddr, QString portNumber);
    bool archonDisconnect(QTcpSocket *socket);
    int archonSend(QTcpSocket *socket, QString preCommand);



private:
    Ui::MainWindow *ui;
    unsigned __int8 msgRef;
    QTcpSocket *socket_1, *socket_2, *socket_3;

};

#endif // MAINWINDOW_H
