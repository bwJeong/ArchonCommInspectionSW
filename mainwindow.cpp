#include "mainwindow.h"
#include "ui_mainwindow.h"

#define BURST_LEN 1024

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    archon_1 = new Archon(this);
    archon_2 = new Archon(this);
    archon_3 = new Archon(this);

    currentFrameCheckTimer = new QTimer(this);
    connect(currentFrameCheckTimer, SIGNAL(timeout()), this, SLOT(checkFrameStatusChange_1()));

    connect(archon_1, SIGNAL(processEvent()), this, SLOT(processEvent()));
    connect(archon_1, SIGNAL(archonSignal(int, QString)), this, SLOT(archonSignalResponse_1(int, QString)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::readConfig(QFile &rFile) {
    QVector<QString> section;

    while(!rFile.atEnd()) {
        QString line = rFile.readLine();

        if (line == '\n') {
            sections_1.push_back(section);
            section.clear();
        }
        else {
            section.push_back(line);
        }
    }

    sections_1.push_back(section);
    section.clear();

    for (int i = 0; i < sections_1.size(); i++) {
        QString sectionName = sections_1[i][0];

        if (sectionName == "[CONFIG]\n") {
            for (int j = 1; j < sections_1[i].size(); j++) {
                QString keyValue = sections_1[i][j];

                int pos = keyValue.indexOf('=');
                QString key = keyValue.left(pos).toUpper().replace("\\", "/");
                QString value = keyValue.remove(0, pos + 1).replace("\"", "").trimmed();

                configKeys_1.push_back(key);
                configValues_1.push_back(value);
            }
        }
    }
}

void MainWindow::checkFrameStatusChange_1() {
    QVector<int> currentFrameStatus = archon_1->newest();

    if (currentFrameStatus[0] != archon_1->getLastFrameStatus()[0]) {
        archon_1->recordCurrentFrameStatus();

        ui->btnExpose_1->setEnabled(true);
        ui->btnExpose_1->setText("Exposure");
        ui->btnFetch_1->setEnabled(true);
        ui->btnFetch_1->setText("Fetch");

        currentFrameCheckTimer->stop();
    }
}

void MainWindow::archonSignalResponse_1(int num, QString str) {
    QString dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

    switch (num) {
    case 0x10:
        ui->lwTx_1->addItem(dateNTime + str);
        ui->lwTx_1->scrollToBottom();
        break;

    case 0x11:
        ui->lwTx_1->addItem(dateNTime + str + "-> Sending error!");
        ui->lwTx_1->scrollToBottom();
        break;

    case 0x20:
        ui->lwRx_1->addItem(dateNTime + str);
        ui->lwRx_1->scrollToBottom();
        break;

    case 0x21:
        ui->lwRx_1->addItem(dateNTime + str + "-> Ack error!");
        ui->lwRx_1->scrollToBottom();
        break;

    case 0x22:
        ui->lwRx_1->addItem(dateNTime + "Response timeout! (> 0.1 s)");
        ui->lwRx_1->scrollToBottom();
        break;

    case 0x30:
        ui->lwRx_1->addItem(dateNTime + str);
        ui->lwRx_1->scrollToBottom();
        break;

    case 0x31:
        ui->lwRx_1->addItem(dateNTime + str + "-> Ack error!");
        ui->lwRx_1->scrollToBottom();
        break;

    case 0x32:
        ui->lwRx_1->addItem(dateNTime + "Response timeout! (> 0.1 s)");
        ui->lwRx_1->scrollToBottom();
        break;
    }

    qApp->processEvents();
}

void MainWindow::processEvent() {
    qApp->processEvents();
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
    readConfig(rFile);

    ui->leConfigFilePath_1->setText(rFile.fileName());
}

void MainWindow::on_btnApplyConfig_1_clicked() {
    QString configCommand;

    archon_1->archonCmd("CLEARCONFIG");

    for (int i = 0; i < configKeys_1.size(); i++) {
        configCommand.sprintf("WCONFIG%04X%s=%s", i, configKeys_1[i].toStdString().c_str(), configValues_1[i].toStdString().c_str());
        archon_1->archonSend(configCommand);
    }

    for (int i = 0 ; i < configKeys_1.size(); i++) { archon_1->minusOneMsgRef(); }
    for (int i = 0; i < configKeys_1.size(); i++) { archon_1->archonRecv(); }

    archon_1->archonCmd("APPLYALL");
    //archon_1->archonCmd("POWERON");
}

void MainWindow::on_btnTgConnection_1_toggled(bool checked) {
    if (checked) {
        if (!archon_1->archonConnect(ui->leIpAddr_1->text(), ui->lePortNumber_1->text())) {
            QMessageBox::warning(this, "Error", "Cannot connect to host!");

            ui->btnTgConnection_1->setChecked(false);
        }
        else {
            ui->leIpAddr_1->setEnabled(false);
            ui->lePortNumber_1->setEnabled(false);
            ui->btnTgConnection_1->setText("Disconnect");
        }
    }
    else {
        archon_1->archonDisconnect();

        ui->leIpAddr_1->setEnabled(true);
        ui->lePortNumber_1->setEnabled(true);
        ui->btnTgConnection_1->setText("Connect");
    }
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

    currentFrameCheckTimer->start(100);
}

void MainWindow::on_btnFetch_1_clicked() {
    QVector<int> lastFrameStatus = archon_1->getLastFrameStatus();
    QString command;
    QString fileName;
    int frameSize;
    int lineSize = BURST_LEN;
    int lines;

    ui->btnFetch_1->setEnabled(false);
    ui->btnFetch_1->setText("Wait...");

    archon_1->archonCmd(command.sprintf("LOCK%d",lastFrameStatus[1] + 1)); // lastFrameStatus[1] = buf

    if (lastFrameStatus[4]) { // lastFrameStatus[4] = sampleSize
        frameSize = 4 * lastFrameStatus[2] * lastFrameStatus[3]; // lastFrameStatus[2] = frameW, lastFrameStatus[3] = frameH
    }
    else {
        frameSize = 2 * lastFrameStatus[2] * lastFrameStatus[3];
    }

    lines = (frameSize + lineSize - 1) / lineSize;
    archon_1->archonSend(command.sprintf("FETCH%08X%08X", ((lastFrameStatus[1] + 1) | 4) << 29, lines));

    QFile imgFile(fileName.sprintf("archon_1_%dx%d_%d_1000ms.raw", lastFrameStatus[2], lastFrameStatus[3], lastFrameStatus[0]));
    int bytesRemaining = frameSize;

    if (!imgFile.open(QFile::WriteOnly|QFile::Truncate)) {
        QMessageBox::warning(this, "Error", "Cannot open write file!");
        ui->btnFetch_1->setText("Fetch");
        return;
    }

    for (int i = 0 ; i < lines; i++) {
        archon_1->minusOneMsgRef();
        imgFile.write(archon_1->archonBinRecv().left(qMin(lineSize, bytesRemaining)).toStdString().c_str());
        bytesRemaining -= lineSize;
    }

    imgFile.close();

    archon_1->plusOneMsgRef();

    ui->btnFetch_1->setText("Complete");
}
