#include "archon.h"

#define BURST_LEN 1024

Archon::Archon() {
    socket = new QTcpSocket();
    configKeys = new QVector<QString>;
    configValues = new QVector<QString>;
    msgRef = 0;
}

bool Archon::archonConnect(QString ipAddr, QString portNumber) {
    if (ipAddr == "" && portNumber == "") { return false; }

    socket->connectToHost(QHostAddress(ipAddr), portNumber.toUShort());

    return socket->waitForConnected();
}

bool Archon::archonDisconnect() {
    socket->disconnectFromHost();

    return socket->waitForDisconnected();
}

qint64 Archon::archonSend(QString preCommand) {
    QString postCommand;

    postCommand.sprintf(">%02X%s\n", msgRef, preCommand.toStdString().c_str());

    return socket->write(postCommand.toStdString().c_str(), postCommand.size());
}

QString Archon::archonRecv() {
    QString reply;
    QTime t;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->canReadLine()) {
            reply = socket->readLine();

            return reply;
        }
    }

    return "";
}

QString Archon::archonBinRecv() {
    QString reply;
    QTime t;
    int binLen = BURST_LEN + 4;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->isReadable() && socket->bytesAvailable() >= binLen) {
            reply = socket->read(binLen);

            return reply;
        }
    }

    return "";
}

void Archon::configClear() {
    configKeys->clear();
    configValues->clear();
}

void Archon::configPush_back(QString key, QString value) {
    configKeys->push_back(key);
    configValues->push_back(value);
}

int Archon::configSize() {
    return configKeys->size();
}

QString Archon::configKey_at(int i) {
    return configKeys->at(i);
}

QString Archon::configValue_at(int i) {
    return configValues->at(i);
}

unsigned __int8 Archon::plusOneMsgRef() {
    return msgRef++;
}
