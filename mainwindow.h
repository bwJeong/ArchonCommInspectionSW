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
    // Connection
    bool archonConnect(QTcpSocket *socket, QString ipAddr, QString portNumber);
    bool archonDisconnect(QTcpSocket *socket);

    // Tx & Rx
    qint64 archonSend(QTcpSocket *socket, QString preCommand);
    QString archonRecv(QTcpSocket *socket);
    QString archonBinRecv(QTcpSocket *socket);

private slots:
    void on_btnReadConfig_1_clicked();

    void on_btnApplyConfig_1_clicked();

private:
    Ui::MainWindow *ui;   
    QTcpSocket *socket_1, *socket_2, *socket_3;
    QVector<QString> *configKeys_1, *configValues_1;
    QVector<QString> *configKeys_2, *configValues_2;
    QVector<QString> *configKeys_3, *configValues_3;

    unsigned __int8 msgRef;
};

#endif // MAINWINDOW_H
