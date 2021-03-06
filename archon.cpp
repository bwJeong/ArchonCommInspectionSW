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

void Archon::archonDisconnect() { socket->disconnectFromHost(); }

void Archon::archonSend(QString preCommand) {
    QString postCommand;

    postCommand.sprintf(">%02X%s\n", msgRef, preCommand.toStdString().c_str());

    if (socket->write(postCommand.toStdString().c_str(), postCommand.size()) != -1) {
        emit archonSignal(0x10, postCommand);
        msgRef++;
    }
    else {
        emit archonSignal(0x11, postCommand);
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
                emit archonSignal(0x20, ack);
                msgRef++;

                return reply.remove(0, 3);
            }
            else {
                emit archonSignal(0x21, ack);

                return "";
            }
        }

        emit processEvent();
    }

    emit archonSignal(0x22);

    return "";
}

QByteArray Archon::archonBinRecv() {
    QByteArray reply;
    QString ack, ackFormat;
    QTime t;
    int binLen = BURST_LEN + 4;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->isReadable() && socket->bytesAvailable() >= binLen) {
            reply = socket->read(binLen);
            ack = reply.left(4);

            if (ack == ackFormat.sprintf("<%02X:", msgRef)) {
                emit archonSignal(0x30, ack);
                msgRef++;

                return reply.remove(0, 4);
            }
            else {
                emit archonSignal(0x31, ack);

                return "";
            }
        }

        emit processEvent();
    }

    emit archonSignal(0x32);

    return "";
}

QString Archon::archonCmd(QString preCommand) {
    // Send
    QString postCommand;

    postCommand.sprintf(">%02X%s\n", msgRef, preCommand.toStdString().c_str());

    if (socket->write(postCommand.toStdString().c_str(), postCommand.size()) != -1) {
        emit archonSignal(0x40, postCommand);
    }
    else {
        emit archonSignal(0x41, postCommand);
    }

    // Recv
    QString reply, ack, ackFormat;
    QTime t;

    t.start();

    while (t.elapsed() <= 5000) {

        if (socket->canReadLine()) {
            reply = socket->readLine();
            ack = reply.left(3);

            if (ack == ackFormat.sprintf("<%02X", msgRef)) {
                emit archonSignal(0x42, ack);
                msgRef++;

                return reply.remove(0, 3);
            }
            else {
                emit archonSignal(0x43, ack);

                return "";
            }
        }

        emit processEvent();
    }

    emit archonSignal(0x44);

    return "";
}

QVector<int> Archon::newest() {
    QMap<QString, int> frameStatus;
    QVector<int> returnValues(5, 0), frames;
    QVector<bool> frameComplete;
    QString archonFormat;
    int rBuf, newestFrame, newestBuf, frameW, frameH, sampleMode;

    QString response = archonCmd("FRAME");

    if (response == "") { return returnValues; }

    QStringList pairs = response.split(' ');

    for (auto pair : pairs) {
        QStringList keyValue = pair.split('=');

        frameStatus[keyValue[0]] = keyValue[1].toInt();
    }

    rBuf = frameStatus["RBUF"] - 1;

    for (int i = 1; i < 4; i++) {
        frames.append(frameStatus[archonFormat.sprintf("BUF%dFRAME", i)]);
        frameComplete.append(frameStatus[archonFormat.sprintf("BUF%dCOMPLETE", i)] == 1);
    }

    if (0 <= rBuf && rBuf <= 2) {
        newestFrame = frames[rBuf];
        newestBuf = rBuf;
    }
    else {
        newestFrame = -1;
        newestBuf = 0;
    }

    for (int i = 0; i < 3; i++) {
        if (frames[i] > newestFrame && frameComplete[i]) {
            newestFrame = frames[i];
            newestBuf = i;
        }
    }

    frameW = frameStatus[archonFormat.sprintf("BUF%dWIDTH", newestBuf + 1)];
    frameH = frameStatus[archonFormat.sprintf("BUF%dHEIGHT", newestBuf + 1)];
    sampleMode = frameStatus[archonFormat.sprintf("BUF%dSAMPLE", newestBuf + 1)];

    returnValues[0] = newestFrame;
    returnValues[1] = newestBuf;
    returnValues[2] = frameW;
    returnValues[3] = frameH;
    returnValues[4] = sampleMode;

    return returnValues;
}

bool Archon::recordCurrentFrameStatus() {
    QVector<int> frameStatus = newest();

    if (frameStatus[0] == 0 && frameStatus[1] == 0 && frameStatus[2] == 0 && frameStatus[3] == 0 && frameStatus[4] == 0) { return false; }
    else {
        lastFrameStatus = frameStatus;

        return true;
    }
}

QVector<int> Archon::getLastFrameStatus() { return lastFrameStatus; }

void Archon::plusOneMsgRef() { msgRef++; }

void Archon::minusOneMsgRef() { msgRef--; }
