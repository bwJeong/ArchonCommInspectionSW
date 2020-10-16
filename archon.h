#ifndef ARCHON_H
#define ARCHON_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QTime>
#include <QVector>
#include <QString>

class Archon {
public:
    Archon();

    // Connection
    bool archonConnect(QString ipAddr, QString portNumber);
    void archonDisconnect();

    // Tx & Rx
    void archonSend(QString preCommand);
    QString archonRecv();
    QString archonBinRecv();
    QString archonCmd(QString preCommand);

signals:
    void archonSendSuccess();
    void archonSendFail();
    void archonRecvSuccess();
    void archonRecvAckError();
    void archonRecvTimeout();
    void archonBinRecvSuccess();
    void archonBinRecvAckError();
    void archonBinRecvTimeout();


private:
    QTcpSocket *socket;
    QVector<QString> *configKeys, *configValues;
    unsigned __int8 msgRef;
};

#endif // ARCHON_H
