#include "mainwindow.h"
#include "ui_mainwindow.h"

#define BURST_LEN 1024

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Initialization
    archon_1 = new Archon(this);
    archon_2 = new Archon(this);
    archon_3 = new Archon(this);
    frameStatusCheckTimer_1 = new QTimer(this);
    frameStatusCheckTimer_2 = new QTimer(this);
    isTxLogSaveFileCreated_1 = false;
    isRxLogSaveFileCreated_1 = false;
    isTxLogSaveFileCreated_2 = false;
    isRxLogSaveFileCreated_2 = false;
    isTxLogSaveFileCreated_3 = false;
    isRxLogSaveFileCreated_3 = false;
    hourCheck = "";

    connect(archon_1, SIGNAL(processEvent()), this, SLOT(processEvent()));
    connect(archon_2, SIGNAL(processEvent()), this, SLOT(processEvent()));
    connect(archon_3, SIGNAL(processEvent()), this, SLOT(processEvent()));
    connect(archon_1, SIGNAL(archonSignal(int, QString)), this, SLOT(archonSignalResponse_1(int, QString)));
    connect(archon_2, SIGNAL(archonSignal(int, QString)), this, SLOT(archonSignalResponse_2(int, QString)));
    connect(archon_3, SIGNAL(archonSignal(int, QString)), this, SLOT(archonSignalResponse_3(int, QString)));
    connect(frameStatusCheckTimer_1, SIGNAL(timeout()), this, SLOT(checkFrameStatusChange_1()));
    connect(frameStatusCheckTimer_2, SIGNAL(timeout()), this, SLOT(checkFrameStatusChange_2()));

    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::readConfig(QFile &rFile, QVector<QVector<QString>> &sections, QVector<QString> &configKeys, QVector<QString> &configValues) {
    // Config parsing
    QVector<QString> section;

    while(!rFile.atEnd()) {
        QString line = rFile.readLine();

        if (line == "\n") {
            sections.push_back(section);
            section.clear();
        }
        else {
            section.push_back(line);
        }
    }

    sections.push_back(section);
    section.clear();

    for (int i = 0; i < sections.size(); i++) {
        QString sectionName = sections[i][0];

        if (sectionName == "[CONFIG]\n") {
            for (int j = 1; j < sections[i].size(); j++) {
                QString keyValue = sections[i][j];

                int pos = keyValue.indexOf('=');
                QString key = keyValue.left(pos).toUpper().replace("\\", "/");
                QString value = keyValue.remove(0, pos + 1).replace("\"", "").trimmed();

                configKeys.push_back(key);
                configValues.push_back(value);
            }
        }
    }
}

void MainWindow::makeTxLogSaveFile_1() {
    QString dateTime = QDateTime::currentDateTime().toString("yyMMddTHHmmss");

    txLogSaveFile_1 = new QFile("Guide_" + dateTime + "_TxLog.txt");
    txLogSaveFile_1->open(QIODevice::WriteOnly);
    isTxLogSaveFileCreated_1 = true;
}

void MainWindow::makeTxLogSaveFile_2() {
    QString dateTime = QDateTime::currentDateTime().toString("yyMMddTHHmmss");

    txLogSaveFile_2 = new QFile("Sci1_" + dateTime + "_TxLog.txt");
    txLogSaveFile_2->open(QIODevice::WriteOnly);
    isTxLogSaveFileCreated_2 = true;
}

void MainWindow::makeTxLogSaveFile_3() {
    QString dateTime = QDateTime::currentDateTime().toString("yyMMddTHHmmss");

    txLogSaveFile_3 = new QFile("Sci2_" + dateTime + "_TxLog.txt");
    txLogSaveFile_3->open(QIODevice::WriteOnly);
    isTxLogSaveFileCreated_3 = true;
}

void MainWindow::makeRxLogSaveFile_1() {
    QString dateTime = QDateTime::currentDateTime().toString("yyMMddTHHmmss");

    rxLogSaveFile_1 = new QFile("Guide_" + dateTime + "_RxLog.txt");
    rxLogSaveFile_1->open(QIODevice::WriteOnly);
    isRxLogSaveFileCreated_1 = true;
}

void MainWindow::makeRxLogSaveFile_2() {
    QString dateTime = QDateTime::currentDateTime().toString("yyMMddTHHmmss");

    rxLogSaveFile_2 = new QFile("Sci1_" + dateTime + "_RxLog.txt");
    rxLogSaveFile_2->open(QIODevice::WriteOnly);
    isRxLogSaveFileCreated_2 = true;
}

void MainWindow::makeRxLogSaveFile_3() {
    QString dateTime = QDateTime::currentDateTime().toString("yyMMddTHHmmss");

    rxLogSaveFile_3 = new QFile("Sci2_" + dateTime + "_RxLog.txt");
    rxLogSaveFile_3->open(QIODevice::WriteOnly);
    isRxLogSaveFileCreated_3 = true;
}

void MainWindow::txLogAutoSave_1(QString txStr) {
    QDateTime dateTime = QDateTime::currentDateTime();

    if ((hourCheck != dateTime.toString("HH")) && (hourCheck != "")) {
        closeTxLogSaveFile_1();
        makeTxLogSaveFile_1();
    }

    hourCheck = dateTime.toString("HH");
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString("HH:mm:ss.zzz");
    txLogSaveFile_1->write(date.toStdString().c_str());
    txLogSaveFile_1->write(" ");
    txLogSaveFile_1->write(time.toStdString().c_str());
    txLogSaveFile_1->write(" ");
    txLogSaveFile_1->write(txStr.toStdString().c_str());
    txLogSaveFile_1->flush();
}

void MainWindow::txLogAutoSave_2(QString txStr) {
    QDateTime dateTime = QDateTime::currentDateTime();

    if ((hourCheck != dateTime.toString("HH")) && (hourCheck != "")) {
        closeTxLogSaveFile_2();
        makeTxLogSaveFile_2();
    }

    hourCheck = dateTime.toString("HH");
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString("HH:mm:ss.zzz");
    txLogSaveFile_2->write(date.toStdString().c_str());
    txLogSaveFile_2->write(" ");
    txLogSaveFile_2->write(time.toStdString().c_str());
    txLogSaveFile_2->write(" ");
    txLogSaveFile_2->write(txStr.toStdString().c_str());
    txLogSaveFile_2->flush();
}

void MainWindow::txLogAutoSave_3(QString txStr) {
    QDateTime dateTime = QDateTime::currentDateTime();

    if ((hourCheck != dateTime.toString("HH")) && (hourCheck != "")) {
        closeTxLogSaveFile_3();
        makeTxLogSaveFile_3();
    }

    hourCheck = dateTime.toString("HH");
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString("HH:mm:ss.zzz");
    txLogSaveFile_3->write(date.toStdString().c_str());
    txLogSaveFile_3->write(" ");
    txLogSaveFile_3->write(time.toStdString().c_str());
    txLogSaveFile_3->write(" ");
    txLogSaveFile_3->write(txStr.toStdString().c_str());
    txLogSaveFile_3->flush();
}

void MainWindow::rxLogAutoSave_1(QString rxStr) {
    QDateTime dateTime = QDateTime::currentDateTime();

    if ((hourCheck != dateTime.toString("HH")) && (hourCheck != "")) {
        closeRxLogSaveFile_1();
        makeRxLogSaveFile_1();
    }

    hourCheck = dateTime.toString("HH");
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString("HH:mm:ss.zzz");
    rxLogSaveFile_1->write(date.toStdString().c_str());
    rxLogSaveFile_1->write(" ");
    rxLogSaveFile_1->write(time.toStdString().c_str());
    rxLogSaveFile_1->write(" ");
    rxLogSaveFile_1->write(rxStr.toStdString().c_str());
    rxLogSaveFile_1->flush();
}

void MainWindow::rxLogAutoSave_2(QString rxStr) {
    QDateTime dateTime = QDateTime::currentDateTime();

    if ((hourCheck != dateTime.toString("HH")) && (hourCheck != "")) {
        closeRxLogSaveFile_2();
        makeRxLogSaveFile_2();
    }

    hourCheck = dateTime.toString("HH");
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString("HH:mm:ss.zzz");
    rxLogSaveFile_2->write(date.toStdString().c_str());
    rxLogSaveFile_2->write(" ");
    rxLogSaveFile_2->write(time.toStdString().c_str());
    rxLogSaveFile_2->write(" ");
    rxLogSaveFile_2->write(rxStr.toStdString().c_str());
    rxLogSaveFile_2->flush();
}

void MainWindow::rxLogAutoSave_3(QString rxStr) {
    QDateTime dateTime = QDateTime::currentDateTime();

    if ((hourCheck != dateTime.toString("HH")) && (hourCheck != "")) {
        closeRxLogSaveFile_3();
        makeRxLogSaveFile_3();
    }

    hourCheck = dateTime.toString("HH");
    QString date = dateTime.toString("yyyy-MM-dd");
    QString time = dateTime.toString("HH:mm:ss.zzz");
    rxLogSaveFile_3->write(date.toStdString().c_str());
    rxLogSaveFile_3->write(" ");
    rxLogSaveFile_3->write(time.toStdString().c_str());
    rxLogSaveFile_3->write(" ");
    rxLogSaveFile_3->write(rxStr.toStdString().c_str());
    rxLogSaveFile_3->flush();
}

void MainWindow::closeTxLogSaveFile_1() {
    isTxLogSaveFileCreated_1 = false;
    txLogSaveFile_1->close();

    delete txLogSaveFile_1;

}

void MainWindow::closeTxLogSaveFile_2() {
    isTxLogSaveFileCreated_2 = false;
    txLogSaveFile_2->close();

    delete txLogSaveFile_2;
}

void MainWindow::closeTxLogSaveFile_3() {
    isTxLogSaveFileCreated_3 = false;
    txLogSaveFile_3->close();

    delete txLogSaveFile_3;
}

void MainWindow::closeRxLogSaveFile_1() {
    isRxLogSaveFileCreated_1 = false;
    rxLogSaveFile_1->close();

    delete rxLogSaveFile_1;
}

void MainWindow::closeRxLogSaveFile_2() {
    isRxLogSaveFileCreated_2 = false;
    rxLogSaveFile_2->close();

    delete rxLogSaveFile_2;
}

void MainWindow::closeRxLogSaveFile_3() {
    isRxLogSaveFileCreated_3 = false;
    rxLogSaveFile_3->close();

    delete rxLogSaveFile_3;
}

void MainWindow::addFITSHeader(QFile &fitsFile, QString key, QString value, QString comment) {
    fitsFile.write(qPrintable(QString("%1= %2 / %3").arg(key, -8).arg(value, 20).arg(comment, -48)), 80);
}

void MainWindow::endFITSHeader(QFile &fitsFile, int lines) {
    int quotient = lines * 80 / 2880;
    int totalLines = (quotient + 1) * 36 - 1;

    fitsFile.write(qPrintable(QString("%1").arg("END", -80)), 80);

    for (lines = totalLines - lines; lines > 0; lines--) {
        fitsFile.write(qPrintable(QString("%1").arg(" ", -80)), 80);
    }
}

void MainWindow::checkFrameStatusChange_1() {
    QVector<int> currentFrameStatus = archon_1->newest();

    if (currentFrameStatus[0] != archon_1->getLastFrameStatus()[0]) {
        archon_1->recordCurrentFrameStatus();

        ui->btnExpose_1->setEnabled(true);
        ui->btnExpose_1->setText("Expose");
        ui->btnFetch_1->setEnabled(true);
        ui->btnFetch_1->setText("Fetch");

        frameStatusCheckTimer_1->stop();
    }
}

void MainWindow::checkFrameStatusChange_2() {
    QVector<int> currentFrameStatus_Sci1 = archon_2->newest();
    QVector<int> currentFrameStatus_Sci2 = archon_3->newest();

    if (currentFrameStatus_Sci1[0] != archon_2->getLastFrameStatus()[0] && currentFrameStatus_Sci2[0] != archon_3->getLastFrameStatus()[0]) {
        archon_2->recordCurrentFrameStatus();
        archon_3->recordCurrentFrameStatus();

        ui->btnExpose_2->setEnabled(true);
        ui->btnExpose_2->setText("Expose");
        ui->btnFetch_2->setEnabled(true);
        ui->btnFetch_2->setText("Fetch");

        frameStatusCheckTimer_2->stop();
    }
}

void MainWindow::archonSignalResponse_1(int num, QString str) {
    QString dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

    switch (num) {
    case 0x10: // archonSend() Success
        if (isTxLogSaveFileCreated_1 == true) { txLogAutoSave_1(str); }
        break;

    case 0x11: // archonSend() Fail
        if (isTxLogSaveFileCreated_1 == true) { txLogAutoSave_1(str); }
        break;

    case 0x20: // archonRecv() Success
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1(str + '\n'); }
        break;

    case 0x21: // archonRecv() Fail
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1(str + "-> Ack error!\n"); }
        break;

    case 0x22: // archonRecv() Timeout
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1("Response timeout! (> 0.1 s)\n"); }
        break;

    case 0x30: // archonBinRecv() Success
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1(str + '\n'); }
        break;

    case 0x31: // archonBinRecv() Fail
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1(str + "-> Ack error!\n"); }
        break;

    case 0x32: // archonBinRecv() Timeout
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1("Response timeout! (> 0.1 s)\n"); }
        break;

    case 0x40: // archonCmd() Send Success
        if (isTxLogSaveFileCreated_1 == true) { txLogAutoSave_1(str); }
        ui->lwTx_1->addItem(dateNTime + str.trimmed());
        ui->lwTx_1->scrollToBottom();
        break;
    case 0x41: // archonCmd() Send Fail
        if (isTxLogSaveFileCreated_1 == true) { txLogAutoSave_1(str.trimmed() + "-> Sending error!\n"); }
        ui->lwTx_1->addItem(dateNTime + str.trimmed() + "-> Sending error!");
        ui->lwTx_1->scrollToBottom();
        break;

    case 0x42: // archonCmd() Recv Success
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1(str + '\n'); }
        ui->lwRx_1->addItem(dateNTime + str);
        ui->lwRx_1->scrollToBottom();
        break;

    case 0x43: // archonCmd() Recv Fail
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1(str + "-> Ack error!\n"); }
        ui->lwRx_1->addItem(dateNTime + str + "-> Ack error!");
        ui->lwRx_1->scrollToBottom();
        break;

    case 0x44: // archonCmd() Recv Timeout
        if (isRxLogSaveFileCreated_1 == true) { rxLogAutoSave_1("Response timeout! (> 5 s)\n"); }
        ui->lwRx_1->addItem(dateNTime + "Response timeout! (> 5 s)");
        ui->lwRx_1->scrollToBottom();
        break;
    }
}

void MainWindow::archonSignalResponse_2(int num, QString str) {
    QString dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

    switch (num) {
    case 0x10: // archonSend() Success
        if (isTxLogSaveFileCreated_2 == true) { txLogAutoSave_2(str); }
        break;

    case 0x11: // archonSend() Fail
        if (isTxLogSaveFileCreated_2 == true) { txLogAutoSave_2(str); }
        break;

    case 0x20: // archonRecv() Success
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2(str + '\n'); }
        break;

    case 0x21: // archonRecv() Fail
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2(str + "-> Ack error!\n"); }
        break;

    case 0x22: // archonRecv() Timeout
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2("Response timeout! (> 0.1 s)\n"); }
        break;

    case 0x30: // archonBinRecv() Success
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2(str + '\n'); }
        break;

    case 0x31: // archonBinRecv() Fail
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2(str + "-> Ack error!\n"); }
        break;

    case 0x32: // archonBinRecv() Timeout
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2("Response timeout! (> 0.1 s)\n"); }
        break;

    case 0x40: // archonCmd() Send Success
        if (isTxLogSaveFileCreated_2 == true) { txLogAutoSave_2(str); }
        ui->lwTx_2->addItem(dateNTime + str.trimmed());
        ui->lwTx_2->scrollToBottom();
        break;
    case 0x41: // archonCmd() Send Fail
        if (isTxLogSaveFileCreated_2 == true) { txLogAutoSave_2(str.trimmed() + "-> Sending error!\n"); }
        ui->lwTx_2->addItem(dateNTime + str.trimmed() + "-> Sending error!");
        ui->lwTx_2->scrollToBottom();
        break;

    case 0x42: // archonCmd() Recv Success
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2(str + '\n'); }
        ui->lwRx_2->addItem(dateNTime + str);
        ui->lwRx_2->scrollToBottom();
        break;

    case 0x43: // archonCmd() Recv Fail
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2(str + "-> Ack error!\n"); }
        ui->lwRx_2->addItem(dateNTime + str + "-> Ack error!");
        ui->lwRx_2->scrollToBottom();
        break;

    case 0x44: // archonCmd() Recv Timeout
        if (isRxLogSaveFileCreated_2 == true) { rxLogAutoSave_2("Response timeout! (> 5 s)\n"); }
        ui->lwRx_2->addItem(dateNTime + "Response timeout! (> 5 s)");
        ui->lwRx_2->scrollToBottom();
        break;
    }
}

void MainWindow::archonSignalResponse_3(int num, QString str) {
    QString dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

    switch (num) {
    case 0x10: // archonSend() Success
        if (isTxLogSaveFileCreated_3 == true) { txLogAutoSave_3(str); }
        break;

    case 0x11: // archonSend() Fail
        if (isTxLogSaveFileCreated_3 == true) { txLogAutoSave_3(str); }
        break;

    case 0x20: // archonRecv() Success
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3(str + '\n'); }
        break;

    case 0x21: // archonRecv() Fail
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3(str + "-> Ack error!\n"); }
        break;

    case 0x22: // archonRecv() Timeout
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3("Response timeout! (> 0.1 s)\n"); }
        break;

    case 0x30: // archonBinRecv() Success
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3(str + '\n'); }
        break;

    case 0x31: // archonBinRecv() Fail
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3(str + "-> Ack error!\n"); }
        break;

    case 0x32: // archonBinRecv() Timeout
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3("Response timeout! (> 0.1 s)\n"); }
        break;

    case 0x40: // archonCmd() Send Success
        if (isTxLogSaveFileCreated_3 == true) { txLogAutoSave_3(str); }
        ui->lwTx_3->addItem(dateNTime + str.trimmed());
        ui->lwTx_3->scrollToBottom();
        break;
    case 0x41: // archonCmd() Send Fail
        if (isTxLogSaveFileCreated_3 == true) { txLogAutoSave_3(str.trimmed() + "-> Sending error!\n"); }
        ui->lwTx_3->addItem(dateNTime + str.trimmed() + "-> Sending error!");
        ui->lwTx_3->scrollToBottom();
        break;

    case 0x42: // archonCmd() Recv Success
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3(str + '\n'); }
        ui->lwRx_3->addItem(dateNTime + str);
        ui->lwRx_3->scrollToBottom();
        break;

    case 0x43: // archonCmd() Recv Fail
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3(str + "-> Ack error!\n"); }
        ui->lwRx_3->addItem(dateNTime + str + "-> Ack error!");
        ui->lwRx_3->scrollToBottom();
        break;

    case 0x44: // archonCmd() Recv Timeout
        if (isRxLogSaveFileCreated_3 == true) { rxLogAutoSave_3("Response timeout! (> 5 s)\n"); }
        ui->lwRx_3->addItem(dateNTime + "Response timeout! (> 5 s)");
        ui->lwRx_3->scrollToBottom();
        break;
    }
}

void MainWindow::processEvent() { qApp->processEvents(); }

void MainWindow::on_btnTgConnection_1_toggled(bool checked) {
    if (checked) {
        if (!archon_1->archonConnect(ui->leIpAddr_1->text(), ui->lePortNumber_1->text())) {
            QMessageBox::warning(this, "Error", "Cannot connect to Guide!");

            ui->btnTgConnection_1->setChecked(false);
        }
        else {
            ui->leIpAddr_1->setEnabled(false);
            ui->lePortNumber_1->setEnabled(false);
            ui->tabWidget->setTabEnabled(1, true);
            ui->btnTgConnection_1->setText("Disconnect");
        }
    }
    else {
        archon_1->archonDisconnect();

        ui->leIpAddr_1->setEnabled(true);
        ui->lePortNumber_1->setEnabled(true);
        ui->tabWidget->setTabEnabled(1, false);
        ui->btnTgConnection_1->setText("Connect");
    }
}

void MainWindow::on_btnTgConnection_2_toggled(bool checked) {
    if (checked) {
        if (!archon_2->archonConnect(ui->leIpAddr_2->text(), ui->lePortNumber_2->text())) {
            QMessageBox::warning(this, "Error", "Cannot connect to Sci_1!");

            ui->btnTgConnection_2->setChecked(false);

            return;
        }

        if (!archon_3->archonConnect(ui->leIpAddr_3->text(), ui->lePortNumber_3->text())) {
            QMessageBox::warning(this, "Error", "Cannot connect to Sci_2!");

            ui->btnTgConnection_2->setChecked(false);

            return;
        }
        else {
            ui->leIpAddr_2->setEnabled(false);
            ui->leIpAddr_3->setEnabled(false);
            ui->lePortNumber_2->setEnabled(false);
            ui->lePortNumber_3->setEnabled(false);
            ui->tabWidget->setTabEnabled(2, true);
            ui->btnTgConnection_2->setText("Disconnect");
        }
    }
    else {
        archon_2->archonDisconnect();
        archon_3->archonDisconnect();

        ui->leIpAddr_2->setEnabled(true);
        ui->leIpAddr_3->setEnabled(true);
        ui->lePortNumber_2->setEnabled(true);
        ui->lePortNumber_3->setEnabled(true);
        ui->tabWidget->setTabEnabled(2, false);
        ui->btnTgConnection_2->setText("Connect");
    }
}

void MainWindow::on_btnReadConfig_1_clicked() {
    // Open config file
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Read file"),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                "acf files (*.acf)"
                );

    if (fileName == "") { return; }

    QFile rFile(fileName);

    if (!rFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open config file!");
        return;
    }

    // Read config file
    sections_1.clear();
    configKeys_1.clear();
    configValues_1.clear();

    readConfig(rFile, sections_1, configKeys_1, configValues_1);

    ui->leConfigFilePath_1->setText(rFile.fileName());
}

void MainWindow::on_btnReadConfig_2_clicked() {
    // Open config file
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Read file"),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                "acf files (*.acf)"
                );

    if (fileName == "") { return; }

    QFile rFile(fileName);

    if (!rFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open config file!");
        return;
    }

    // Read config file
    sections_2.clear();
    configKeys_2.clear();
    configValues_2.clear();

    readConfig(rFile, sections_2, configKeys_2, configValues_2);

    ui->leConfigFilePath_2->setText(rFile.fileName());
}

void MainWindow::on_btnApplyConfig_1_clicked() {
    QString configCommand;

    if (ui->leConfigFilePath_1->text() != "") {
        ui->btnApplyConfig_1->setEnabled(false);
        ui->btnApplyConfig_1->setText("Working...");

        archon_1->archonCmd("CLEARCONFIG");

        for (int i = 0; i < configKeys_1.size(); i++) {
            configCommand.sprintf("WCONFIG%04X%s=%s", i, configKeys_1[i].toStdString().c_str(), configValues_1[i].toStdString().c_str());
            archon_1->archonSend(configCommand);
        }

        for (int i = 0 ; i < configKeys_1.size(); i++) { archon_1->minusOneMsgRef(); }
        for (int i = 0; i < configKeys_1.size(); i++) { archon_1->archonRecv(); }

        archon_1->archonCmd("APPLYALL");

        ui->btnApplyConfig_1->setEnabled(true);
        ui->btnApplyConfig_1->setText("Apply config file");
    }
    else {
        QMessageBox::warning(this, "Error", "Please read the config file before applying");
    }
}

void MainWindow::on_btnApplyConfig_2_clicked() {
    QString configCommand;

    if (ui->leConfigFilePath_2->text() != "") {
        ui->btnApplyConfig_2->setEnabled(false);
        ui->btnApplyConfig_2->setText("Working...");

        archon_2->archonCmd("CLEARCONFIG");
        archon_3->archonCmd("CLEARCONFIG");

        for (int i = 0; i < configKeys_2.size(); i++) {
            configCommand.sprintf("WCONFIG%04X%s=%s", i, configKeys_2[i].toStdString().c_str(), configValues_2[i].toStdString().c_str());
            archon_2->archonSend(configCommand);
            archon_3->archonSend(configCommand);
        }

        for (int i = 0 ; i < configKeys_2.size(); i++) {
            archon_2->minusOneMsgRef();
            archon_3->minusOneMsgRef();
        }
        for (int i = 0; i < configKeys_2.size(); i++) {
            archon_2->archonRecv();
            archon_3->archonRecv();
        }

        archon_2->archonCmd("APPLYALL");
        archon_3->archonCmd("APPLYALL");

        ui->btnApplyConfig_2->setEnabled(true);
        ui->btnApplyConfig_2->setText("Apply config file");
    }
    else {
        QMessageBox::warning(this, "Error", "Please read the config file before applying");
    }
}

void MainWindow::on_btnCcdPwrOn_1_clicked() {
    archon_1->archonCmd("POWERON");
}

void MainWindow::on_btnCcdPwrOn_2_clicked() {
    archon_2->archonCmd("POWERON");
    archon_3->archonCmd("POWERON");
}

void MainWindow::on_btnCcdPwrOff_1_clicked() {
    archon_1->archonCmd("POWEROFF");
}

void MainWindow::on_btnCcdPwrOff_2_clicked() {
    archon_2->archonCmd("POWEROFF");
    archon_3->archonCmd("POWEROFF");
}

void MainWindow::on_btnExpose_1_clicked() {
    QString configCommand;
    int configLineNum;

    if (!archon_1->recordCurrentFrameStatus()) {
        QMessageBox::warning(this, "Error", "Cannot read current frame status!");

        return;
    }

    ui->btnExpose_1->setEnabled(false);
    ui->btnExpose_1->setText("Wait...");

    configLineNum = configKeys_1.indexOf("PARAMETER2");
    //configValues_1[configLineNum] = "IntMS=1000";
    archon_1->archonCmd(configCommand.sprintf("WCONFIG%04X%s=%s", configLineNum, "PARAMETER2", "IntMS=1000"));

    configLineNum = configKeys_1.indexOf("PARAMETER1");
    //configValues_1[configLineNum] = "Exposures=1";
    archon_1->archonCmd(configCommand.sprintf("WCONFIG%04X%s=%s", configLineNum, "PARAMETER1", "Exposures=1"));
    archon_1->archonCmd("LOADPARAMS");

    fitsDateTime = QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss");

    frameStatusCheckTimer_1->start(500);
}

void MainWindow::on_btnExpose_2_clicked() {
    QString configCommand;
    int configLineNum;

    if (!(archon_2->recordCurrentFrameStatus() && archon_3->recordCurrentFrameStatus())) {
        QMessageBox::warning(this, "Error", "Cannot read current frame status!");

        return;
    }

    ui->btnExpose_2->setEnabled(false);
    ui->btnExpose_2->setText("Wait...");

    configLineNum = configKeys_2.indexOf("PARAMETER2");
    //configValues_1[configLineNum] = "IntMS=1000";
    archon_2->archonCmd(configCommand.sprintf("WCONFIG%04X%s=%s", configLineNum, "PARAMETER2", "IntMS=1000"));
    archon_3->archonCmd(configCommand.sprintf("WCONFIG%04X%s=%s", configLineNum, "PARAMETER2", "IntMS=1000"));

    configLineNum = configKeys_2.indexOf("PARAMETER1");
    //configValues_1[configLineNum] = "Exposures=1";
    archon_2->archonCmd(configCommand.sprintf("WCONFIG%04X%s=%s", configLineNum, "PARAMETER1", "Exposures=1"));
    archon_3->archonCmd(configCommand.sprintf("WCONFIG%04X%s=%s", configLineNum, "PARAMETER1", "Exposures=1"));

    archon_2->archonCmd("LOADPARAMS");
    archon_3->archonCmd("LOADPARAMS");

    fitsDateTime = QDateTime::currentDateTime().toString("yyyy-MM-ddTHH:mm:ss");

    frameStatusCheckTimer_2->start(500);
}

void MainWindow::on_btnFetch_1_clicked() {
    QString command;
    QString fileName;
    int frameSize;
    int lineSize = BURST_LEN;
    int lines;

    statusKeys_1.clear();
    statusValues_1.clear();

    ui->btnFetch_1->setEnabled(false);
    ui->btnFetch_1->setText("Wait...");

    // Read archon status
    QString response = archon_1->archonCmd("STATUS");

    if (response == "") { return; }

    QStringList pairs = response.trimmed().split(' ');

    for (auto pair : pairs) {
        QString str;
        QStringList keyValue = pair.split('=');

        statusKeys_1.push_back(keyValue[0]);
        statusValues_1.push_back(keyValue[1]);
    }

    // Fetch
    //////////////////////////////////////
    /// \brief lastFrameStatus
    /// lastFrameStatus[0] -> frame
    /// lastFrameStatus[1] -> buf
    /// lastFrameStatus[2] -> frameW
    /// lastFrameStatus[3] -> frameH
    /// lastFrameStatus[4] -> sampleSize
    //////////////////////////////////////

    QVector<int> lastFrameStatus = archon_1->getLastFrameStatus();

    archon_1->archonCmd(command.sprintf("LOCK%d",lastFrameStatus[1] + 1));

    if (lastFrameStatus[4]) {
        frameSize = 4 * lastFrameStatus[2] * lastFrameStatus[3];
    }
    else {
        frameSize = 2 * lastFrameStatus[2] * lastFrameStatus[3];
    }

    lines = (frameSize + lineSize - 1) / lineSize;
    archon_1->archonSend(command.sprintf("FETCH%08X%08X", ((lastFrameStatus[1] + 1) | 4) << 29, lines));

    // Make FITS file
    QFile fitsFile(fileName.sprintf("Guide_%dx%d_1000ms_%d.fits", lastFrameStatus[2], lastFrameStatus[3], lastFrameStatus[0]));
    int bytesRemaining = frameSize;

    if (!fitsFile.open(QFile::WriteOnly|QFile::Truncate)) {
        QMessageBox::warning(this, "Error", "Cannot open write file!");

        ui->btnFetch_1->setText("Fetch");

        return;
    }

    // Add FITS header
    int headerlines = 0;

    addFITSHeader(fitsFile, "SIMPLE", "T", "Conform to FITS standard"); headerlines++;
    addFITSHeader(fitsFile, "BITPIX", "16", "Unsigned short data"); headerlines++;
    addFITSHeader(fitsFile, "NAXIS", "2", "Number of axes"); headerlines++;
    addFITSHeader(fitsFile, "NAXIS1", QString::number(lastFrameStatus[2]), "Image width"); headerlines++;
    addFITSHeader(fitsFile, "NAXIS2", QString::number(lastFrameStatus[3]), "Image height"); headerlines++;
    addFITSHeader(fitsFile, "BZERO", "32768", "Offset for unsigned short"); headerlines++;
    addFITSHeader(fitsFile, "BSCALE", "1", "Default scaling factor"); headerlines++;
    addFITSHeader(fitsFile, "DATETIME", fitsDateTime, "Date & Time"); headerlines++;

    for (int i = 0; i < statusKeys_1.size(); i++) {
        addFITSHeader(fitsFile, statusKeys_1[i], statusValues_1[i], "Archon status"); headerlines++;
    }

    endFITSHeader(fitsFile, headerlines);

    // Write image
    for (int i = 0; i < lines; i++) {
        archon_1->minusOneMsgRef();

        QByteArray raw = archon_1->archonBinRecv().left(qMin(lineSize, bytesRemaining));

        fitsFile.write(raw);
        bytesRemaining -= lineSize;
    }

    archon_1->plusOneMsgRef();
    fitsFile.close();

    ui->btnFetch_1->setText("Complete");
}

void MainWindow::on_btnFetch_2_clicked() {
    QString command;
    QString fileName;
    int frameSize;
    int lineSize = BURST_LEN;
    int lines;

    statusKeys_2.clear();
    statusValues_2.clear();
    statusKeys_3.clear();
    statusValues_3.clear();

    ui->btnFetch_2->setEnabled(false);
    ui->btnFetch_2->setText("Wait...");

    // Read archon status
    QString response_Sci1 = archon_2->archonCmd("STATUS");
    QString response_Sci2 = archon_3->archonCmd("STATUS");

    if (response_Sci1 == "" || response_Sci2 == "") { return; }

    QStringList pairs_Sci1 = response_Sci1.trimmed().split(' ');
    QStringList pairs_Sci2 = response_Sci2.trimmed().split(' ');

    for (auto pair : pairs_Sci1) {
        QString str;
        QStringList keyValue = pair.split('=');

        statusKeys_2.push_back(keyValue[0]);
        statusValues_2.push_back(keyValue[1]);
    }

    for (auto pair : pairs_Sci2) {
        QString str;
        QStringList keyValue = pair.split('=');

        statusKeys_3.push_back(keyValue[0]);
        statusValues_3.push_back(keyValue[1]);
    }

    // Fetch
    QVector<int> lastFrameStatus_Sci1 = archon_2->getLastFrameStatus();
    QVector<int> lastFrameStatus_Sci2 = archon_3->getLastFrameStatus();

    archon_2->archonCmd(command.sprintf("LOCK%d",lastFrameStatus_Sci1[1] + 1)); // lastFrameStatus[1] = buf
    archon_3->archonCmd(command.sprintf("LOCK%d",lastFrameStatus_Sci2[1] + 1)); // lastFrameStatus[1] = buf

    if (lastFrameStatus_Sci1[4]) { // lastFrameStatus[4] = sampleSize
        frameSize = 4 * lastFrameStatus_Sci1[2] * lastFrameStatus_Sci1[3]; // lastFrameStatus[2] = frameW, lastFrameStatus[3] = frameH
    }
    else {
        frameSize = 2 * lastFrameStatus_Sci1[2] * lastFrameStatus_Sci1[3];
    }

    lines = (frameSize + lineSize - 1) / lineSize;
    archon_2->archonSend(command.sprintf("FETCH%08X%08X", ((lastFrameStatus_Sci1[1] + 1) | 4) << 29, lines));
    archon_3->archonSend(command.sprintf("FETCH%08X%08X", ((lastFrameStatus_Sci2[1] + 1) | 4) << 29, lines));

    // Make FITS file
    QFile fitsFile_Sci1(fileName.sprintf("Sci1_%dx%d_1000ms_%d.fits", lastFrameStatus_Sci1[2], lastFrameStatus_Sci1[3], lastFrameStatus_Sci1[0]));
    QFile fitsFile_Sci2(fileName.sprintf("Sci2_%dx%d_1000ms_%d.fits", lastFrameStatus_Sci2[2], lastFrameStatus_Sci2[3], lastFrameStatus_Sci2[0]));
    int bytesRemaining = frameSize;

    if (!(fitsFile_Sci1.open(QFile::WriteOnly|QFile::Truncate) && fitsFile_Sci2.open(QFile::WriteOnly|QFile::Truncate))) {
        QMessageBox::warning(this, "Error", "Cannot open write file!");

        ui->btnFetch_1->setText("Fetch");

        return;
    }

    // Add FITS header
    int headerlines = 0;

    addFITSHeader(fitsFile_Sci1, "SIMPLE", "T", "Conform to FITS standard"); headerlines++;
    addFITSHeader(fitsFile_Sci1, "BITPIX", "16", "Unsigned short data"); headerlines++;
    addFITSHeader(fitsFile_Sci1, "NAXIS", "2", "Number of axes"); headerlines++;
    addFITSHeader(fitsFile_Sci1, "NAXIS1", QString::number(lastFrameStatus_Sci1[2]), "Image width"); headerlines++;
    addFITSHeader(fitsFile_Sci1, "NAXIS2", QString::number(lastFrameStatus_Sci1[3]), "Image height"); headerlines++;
    addFITSHeader(fitsFile_Sci1, "BZERO", "32768", "Offset for unsigned short"); headerlines++;
    addFITSHeader(fitsFile_Sci1, "BSCALE", "1", "Default scaling factor"); headerlines++;
    addFITSHeader(fitsFile_Sci1, "DATETIME", fitsDateTime, "Date & Time"); headerlines++;

    for (int i = 0; i < statusKeys_2.size(); i++) {
        addFITSHeader(fitsFile_Sci1, statusKeys_2[i], statusValues_2[i], "Archon status"); headerlines++;
    }

    endFITSHeader(fitsFile_Sci1, headerlines);

    headerlines = 0;

    addFITSHeader(fitsFile_Sci2, "SIMPLE", "T", "Conform to FITS standard"); headerlines++;
    addFITSHeader(fitsFile_Sci2, "BITPIX", "16", "Unsigned short data"); headerlines++;
    addFITSHeader(fitsFile_Sci2, "NAXIS", "2", "Number of axes"); headerlines++;
    addFITSHeader(fitsFile_Sci2, "NAXIS1", QString::number(lastFrameStatus_Sci2[2]), "Image width"); headerlines++;
    addFITSHeader(fitsFile_Sci2, "NAXIS2", QString::number(lastFrameStatus_Sci2[3]), "Image height"); headerlines++;
    addFITSHeader(fitsFile_Sci2, "BZERO", "32768", "Offset for unsigned short"); headerlines++;
    addFITSHeader(fitsFile_Sci2, "BSCALE", "1", "Default scaling factor"); headerlines++;
    addFITSHeader(fitsFile_Sci2, "DATETIME", fitsDateTime, "Date & Time"); headerlines++;

    for (int i = 0; i < statusKeys_3.size(); i++) {
        addFITSHeader(fitsFile_Sci2, statusKeys_3[i], statusValues_3[i], "Archon status"); headerlines++;
    }

    endFITSHeader(fitsFile_Sci2, headerlines);

    // Write image

    for (int i = 0; i < lines; i++) {
        archon_2->minusOneMsgRef();
        archon_3->minusOneMsgRef();

        QByteArray raw_Sci1 = archon_2->archonBinRecv().left(qMin(lineSize, bytesRemaining));
        QByteArray raw_Sci2 = archon_3->archonBinRecv().left(qMin(lineSize, bytesRemaining));

        fitsFile_Sci1.write(raw_Sci1);
        fitsFile_Sci2.write(raw_Sci2);

        bytesRemaining -= lineSize;
    }

    archon_2->plusOneMsgRef();
    archon_3->plusOneMsgRef();

    fitsFile_Sci1.close();
    fitsFile_Sci2.close();

    ui->btnFetch_2->setText("Complete");
}

void MainWindow::on_btnTgTxLogAutoSave_1_toggled(bool checked) {
    if (checked) {
        makeTxLogSaveFile_1();
        ui->btnTgTxLogAutoSave_1->setText("Auto saving...");
    }
    else {
        closeTxLogSaveFile_1();
        ui->btnTgTxLogAutoSave_1->setText("Auto save");
    }
}

void MainWindow::on_btnTgTxLogAutoSave_2_toggled(bool checked) {
    if (checked) {
        makeTxLogSaveFile_2();
        ui->btnTgTxLogAutoSave_2->setText("Auto saving...");
    }
    else {
        closeTxLogSaveFile_2();
        ui->btnTgTxLogAutoSave_2->setText("Auto save");
    }
}

void MainWindow::on_btnTgTxLogAutoSave_3_toggled(bool checked) {
    if (checked) {
        makeTxLogSaveFile_3();
        ui->btnTgTxLogAutoSave_3->setText("Auto saving...");
    }
    else {
        closeTxLogSaveFile_3();
        ui->btnTgTxLogAutoSave_3->setText("Auto save");
    }
}

void MainWindow::on_btnTgRxLogAutoSave_1_toggled(bool checked) {
    if (checked) {
        makeRxLogSaveFile_1();
        ui->btnTgRxLogAutoSave_1->setText("Auto saving...");
    }
    else {
        closeRxLogSaveFile_1();
        ui->btnTgRxLogAutoSave_1->setText("Auto save");
    }
}

void MainWindow::on_btnTgRxLogAutoSave_2_toggled(bool checked) {
    if (checked) {
        makeRxLogSaveFile_2();
        ui->btnTgRxLogAutoSave_2->setText("Auto saving...");
    }
    else {
        closeRxLogSaveFile_2();
        ui->btnTgRxLogAutoSave_2->setText("Auto save");
    }
}

void MainWindow::on_btnTgRxLogAutoSave_3_toggled(bool checked) {
    if (checked) {
        makeRxLogSaveFile_3();
        ui->btnTgRxLogAutoSave_3->setText("Auto saving...");
    }
    else {
        closeRxLogSaveFile_3();
        ui->btnTgRxLogAutoSave_3->setText("Auto save");
    }
}
