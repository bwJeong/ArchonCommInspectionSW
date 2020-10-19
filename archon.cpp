#include "archon.h"

#define BURST_LEN 1024

Archon::Archon(QObject *parent) : QObject(parent) {
    socket = new QTcpSocket();
    msgRef = 0;
}

bool Archon::archonConnect(QString ipAddr, QString portNumber) {
    if (ipAddr == "" && portNumber == "") { return false; }

    socket->connectToHost(QHostAddress(ipAddr), portNumber.toUShort());

    return socket->waitForConnected();
}

void Archon::archonDisconnect() {
    socket->disconnectFromHost();
}

void Archon::archonSend(QString preCommand) {
    QString postCommand;

    postCommand.sprintf(">%02X%s\n", msgRef, preCommand.toStdString().c_str());

    if (socket->write(postCommand.toStdString().c_str(), postCommand.size()) != -1) {
        emit archonSendSuccess();
        msgRef++;
    }
    else {
        emit archonSendFail();
    }
}

QString Archon::archonRecv() {
    QString reply, ack, ackFormat;
    QTime t;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->canReadLine()) {
            reply = socket->readLine();
            ack = reply.left(3);

            if (ack == ackFormat.sprintf("<%02X", msgRef)) {
                emit archonRecvSuccess();
                msgRef++;

                return reply.remove(0, 3);
            }
            else {
                emit archonRecvAckError();

                return "";
            }
        }
    }

    emit archonRecvTimeout();

    return "";
}

QString Archon::archonBinRecv() {
    QString reply, ack, ackFormat;
    QTime t;
    int binLen = BURST_LEN + 4;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->isReadable() && socket->bytesAvailable() >= binLen) {
            reply = socket->read(binLen);
            ack = reply.left(4);

            if (ack == ackFormat.sprintf("<%02X:", msgRef)) {
                emit archonBinRecvSuccess();
                msgRef++;

                return reply.remove(0, 4);
            }
            else {
                emit archonBinRecvAckError();

                return "";
            }
        }
    }

    emit archonBinRecvTimeout();

    return "";
}

QString Archon::archonCmd(QString preCommand) {
    // Send
    QString postCommand;

    postCommand.sprintf(">%02X%s\n", msgRef, preCommand.toStdString().c_str());

    if (socket->write(postCommand.toStdString().c_str(), postCommand.size()) != -1) {
        emit archonSendSuccess();
    }
    else {
        emit archonSendFail();
    }

    // Recv
    QString reply, ack, ackFormat;
    QTime t;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->canReadLine()) {
            reply = socket->readLine();
            ack = reply.left(3);

            if (ack == ackFormat.sprintf("<%02X", msgRef)) {
                emit archonRecvSuccess();
                msgRef++;

                return reply.remove(0, 3);
            }
            else {
                emit archonRecvAckError();

                return "";
            }
        }
    }

    emit archonRecvTimeout();

    return "";
}

void Archon::minusOneMsgRef() {
    msgRef--;
}
