#ifndef ARCHON_H
#define ARCHON_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTime>
#include <QString>

class Archon : public QObject {
    Q_OBJECT
public:
    explicit Archon(QObject *parent = nullptr);

    // Connection
    bool archonConnect(QString ipAddr, QString portNumber);
    void archonDisconnect();

    // Tx & Rx
    void archonSend(QString preCommand);
    QString archonRecv();
    QString archonBinRecv();
    QString archonCmd(QString preCommand);

    void minusOneMsgRef();

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
    unsigned __int8 msgRef;
};

#endif // ARCHON_H
