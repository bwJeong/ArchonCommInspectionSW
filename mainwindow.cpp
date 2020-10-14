#include "mainwindow.h"
#include "ui_mainwindow.h"

#define BURST_LEN 1024

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    msgRef = 0;

    socket_1 = new QTcpSocket();
    socket_2 = new QTcpSocket();
    socket_3 = new QTcpSocket();

    configKeys_1 = new QVector<QString>;
    configValues_1 = new QVector<QString>;
    configKeys_2 = new QVector<QString>;
    configValues_2 = new QVector<QString>;
    configKeys_3 = new QVector<QString>;
    configValues_3 = new QVector<QString>;
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

qint64 MainWindow::archonSend(QTcpSocket *socket, QString preCommand) {
    QString postCommand;

    postCommand.sprintf(">%02X%s\n", msgRef, preCommand.toStdString().c_str());

    return socket->write(postCommand.toStdString().c_str(), postCommand.size());
}

QString MainWindow::archonRecv(QTcpSocket *socket) {
    QString reply;
    QTime t;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->canReadLine()) {
            reply = socket->readLine();

            return reply;
        }

        qApp->processEvents();
    }

    return "";
}

QString MainWindow::archonBinRecv(QTcpSocket *socket) {
    QString reply;
    QTime t;
    int binLen = BURST_LEN + 4;

    t.start();

    while (t.elapsed() <= 100) {

        if (socket->isReadable() && socket->bytesAvailable() >= binLen) {
            reply = socket->read(binLen);

            return reply;
        }

        qApp->processEvents();
    }

    return "";
}

void MainWindow::on_btnReadConfig_1_clicked() {
    // Open config file
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Read file"),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                "acf files (*.acf)"
                );

    if (fileName == "") {
        return;
    }

    QFile rFile(fileName);

    // Read Config file
    bool sectionTrigger = false;

    configKeys_1->clear();
    configValues_1->clear();

    if (!rFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open config file!");
    }

    while (!rFile.atEnd()) {
        QString line = rFile.readLine();

        if (sectionTrigger == true) {
            int pos = line.indexOf('=');
            QString key = line.left(pos).toUpper().replace("\\", "/");
            QString value = line.remove(0, pos + 1).replace("\"", "").trimmed();

            configKeys_1->push_back(key);
            configValues_1->push_back(value);
        }

        if (line == "[CONFIG]\n") { sectionTrigger = true; }
    }

    rFile.close();

    ui->leConfigFilePath_1->setText(fileName);
}

void MainWindow::on_btnApplyConfig_1_clicked() {
    QString dateNTime;

    // Send "CLEARCONFIG" command & Receive ack
    if (archonSend(socket_1, "CLEARCONFIG") != -1) {
        dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

        ui->lwTx_1->addItem(dateNTime + "CLEARCONFIG");

        QString rBuf = archonRecv(socket_1);

        if (rBuf != "") {
            QString ackFormat;
            QString ack = rBuf.left(3);

            if (ack == ackFormat.sprintf("<%02X", msgRef++)) {
                dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

                ui->lwRx_1->addItem(dateNTime + ack);
            }
            else {
                dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

                ui->lwRx_1->addItem(dateNTime + "Ack error! (Returned: " + ack + " Expected: " + ackFormat + ")");
            }
        }
        else {
            dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

            ui->lwRx_1->addItem(dateNTime + "Response timeout! (> 0.1 s)");
        }
    }
    else {
        dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

        ui->lwTx_1->addItem(dateNTime + "Command sending error!");
    }

    // Send config command
    QString configCommand;

    for (int i = 0; i < configKeys_1->size(); i++) {
        configCommand.sprintf("WCONFIG%04X%s=%s", i, configKeys_1->at(i).toStdString().c_str(), configValues_1->at(i).toStdString().c_str());

        if (archonSend(socket_1, configCommand) != -1) {
            dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

            ui->lwTx_1->addItem(dateNTime + configCommand);
        }
        else {
            dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

            ui->lwTx_1->addItem(dateNTime + "Command sending error!");
        }

    }

    // Receive ack

}
