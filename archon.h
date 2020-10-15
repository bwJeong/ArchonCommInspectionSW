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

    bool archonConnect(QString ipAddr, QString portNumber);
    bool archonDisconnect();
    qint64 archonSend(QString preCommand);
    QString archonRecv();
    QString archonBinRecv();

    void configClear();
    void configPush_back(QString key, QString value);
    int configSize();
    QString configKey_at(int i);
    QString configValue_at(int i);

    unsigned __int8 plusOneMsgRef();

private:
    QTcpSocket *socket;
    QVector<QString> *configKeys, *configValues;
    unsigned __int8 msgRef;
};

#endif // ARCHON_H
