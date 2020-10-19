#ifndef ARCHON_H
#define ARCHON_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTime>
#include <QString>
#include <QVector>
#include <QMap>

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

    // Frame
    QVector<int> newest();
    bool recordCurrentFrameStatus();
    QVector<int> getLastFrameStatus();

    // Etc.
    void plusOneMsgRef();
    void minusOneMsgRef();

signals:
    void archonSignal(int num, QString str = "");
    void processEvent();

private:
    QTcpSocket *socket;
    unsigned __int8 msgRef;
    QVector<int> lastFrameStatus;
};

#endif // ARCHON_H
