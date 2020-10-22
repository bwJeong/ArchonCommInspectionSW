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

    connect(archon_1, SIGNAL(processEvent()), this, SLOT(processEvent()));
    connect(archon_1, SIGNAL(archonSignal(int, QString)), this, SLOT(archonSignalResponse_1(int, QString)));

    frameStatusCheckTimer = new QTimer(this);

    connect(frameStatusCheckTimer, SIGNAL(timeout()), this, SLOT(checkFrameStatusChange_1()));

    makeSaveDirectory();

    QString saveDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/ARCHONCOMMINSPECTIONSW_SAVEFILES";

    ui->leLogSavePath_1->setText(saveDirectoryPath);
    ui->leRawFileSavePath_1->setText(saveDirectoryPath);

    isTxLogSaveFileCreated_1 = false;
    isRxLogSaveFileCreated_1 = false;
    hourCheck = "";
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

void MainWindow::makeSaveDirectory() {
    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir saveDirectory(documentsPath + "/ARCHONCOMMINSPECTIONSW_SAVEFILES");

    saveDirectory.mkpath(documentsPath + "/ARCHONCOMMINSPECTIONSW_SAVEFILES");
}

void MainWindow::makeTxLogSaveFile_1() {
    QString saveDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/ARCHONCOMMINSPECTIONSW_SAVEFILES";
    QString fileName = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");

    txLogSaveFile_1 = new QFile(saveDirectoryPath + "/archon1_" +fileName + "_TxLog.txt");
    txLogSaveFile_1->open(QIODevice::WriteOnly);
    isTxLogSaveFileCreated_1 = true;
}

void MainWindow::makeRxLogSaveFile_1() {
    QString saveDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/ARCHONCOMMINSPECTIONSW_SAVEFILES";
    QString fileName = QDateTime::currentDateTime().toString("yyMMdd_HHmmss");

    rxLogSaveFile_1 = new QFile(saveDirectoryPath + "/archon1_" + fileName + "_RxLog.txt");
    rxLogSaveFile_1->open(QIODevice::WriteOnly);
    isRxLogSaveFileCreated_1 = true;
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

void MainWindow::closeTxLogSaveFile_1() {
    isTxLogSaveFileCreated_1 = false;
    txLogSaveFile_1->close();

    delete txLogSaveFile_1;

}

void MainWindow::closeRxLogSaveFile_1() {
    isRxLogSaveFileCreated_1 = false;
    rxLogSaveFile_1->close();

    delete rxLogSaveFile_1;
}

void MainWindow::addFITSHeader(QFile &fitsFile, QString key, QString value, QString comment) {
    fitsFile.write(qPrintable(QString("%1= %2 / %3").arg(key, -8).arg(value, 20).arg(comment, -48)), 80);
}

void MainWindow::endFITSHeader(QFile &fitsFile, int lines) {
    fitsFile.write(qPrintable(QString("%1").arg("END", -80)), 80);

    for (lines = 35 - lines; lines > 0; lines--) {
        fitsFile.write(qPrintable(QString("%1").arg(" ", -80)), 80);
    }
}

void MainWindow::saveFITS(QFile &rawFile, const int w, const int h) {
    int i, x, y, linesize, headerlines;
    QVector<quint16> buf;
    quint16 *linebuf;
    char pad[2880];
    QString s;

    // Read raw file
    while (!rawFile.atEnd()) {
        QByteArray temp = rawFile.read(2);
        quint16 elem = qToBigEndian((quint16)((temp[0] << 8) | (temp[1])));

        buf.push_back(elem);
    }

    // Fill FITS padding with zeroes
    for (y = 0; y < 2880; y++)
        pad[y] = 0;

    // Create file name
    s = rawFile.fileName().split(".")[0] + ".fits";

    // Save file
    QFile fitsFile(s);

    if (!fitsFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Error", "Cannot open write file!");

        return;
    }

    linesize = w * 2;

    // Write primary FITS header
    headerlines = 0;

    addFITSHeader(fitsFile, "SIMPLE", "T", "Conform to FITS standard"); headerlines++;
    addFITSHeader(fitsFile, "BITPIX", "16", "Unsigned short data"); headerlines++;
    addFITSHeader(fitsFile, "NAXIS", "2", "Number of axes"); headerlines++;
    addFITSHeader(fitsFile, "NAXIS1", QString::number(w), "Image width"); headerlines++;
    addFITSHeader(fitsFile, "NAXIS2", QString::number(h), "Image height"); headerlines++;
    addFITSHeader(fitsFile, "BZERO", "32768", "Offset for unsigned short"); headerlines++;
    addFITSHeader(fitsFile, "BSCALE", "1", "Default scaling factor"); headerlines++;
    endFITSHeader(fitsFile, headerlines);

    // Write image data (byte order must be swapped)
    linebuf = new quint16[w];

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            linebuf[x] = qToBigEndian((quint16)(buf[x + y * w] ^ 0x8000));

            qApp->processEvents();
        }

        if (fitsFile.write((char *)linebuf, linesize) != linesize) {
            QMessageBox::warning(this, "Error", "Cannot write fits file!");

            rawFile.close();
            fitsFile.close();

            return;
        }

    }

    delete[] linebuf;

    // Pad to end of FITS block
    i = 2880 - (w * h * 2) % 2880;

    if (i != 2880) {
        if (fitsFile.write(pad, i) != i) {
            printf("Cannot write fits file! [err code: 02]\n");

            rawFile.close();
            fitsFile.close();

            return;
        }
    }

    rawFile.close();
    fitsFile.close();
}

void MainWindow::checkFrameStatusChange_1() {
    QVector<int> currentFrameStatus = archon_1->newest();

    if (currentFrameStatus[0] != archon_1->getLastFrameStatus()[0]) {
        archon_1->recordCurrentFrameStatus();

        ui->btnExpose_1->setEnabled(true);
        ui->btnExpose_1->setText("Expose");
        ui->btnFetch_1->setEnabled(true);
        ui->btnFetch_1->setText("Fetch");

        frameStatusCheckTimer->stop();
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

    qApp->processEvents();
}

void MainWindow::processEvent() { qApp->processEvents(); }

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
    //archon_1->archonCmd("POWERON");

    ui->btnApplyConfig_1->setEnabled(true);
    ui->btnApplyConfig_1->setText("Apply config file");
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

    frameStatusCheckTimer->start(500);
}

void MainWindow::on_btnFetch_1_clicked() {
    // Fetch
    QString saveDirectoryPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/ARCHONCOMMINSPECTIONSW_SAVEFILES";
    QString command;
    QString fileName;
    int frameSize;
    int lineSize = BURST_LEN;
    int lines;

    QVector<int> lastFrameStatus = archon_1->getLastFrameStatus();

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

    QFile imgFile(saveDirectoryPath + fileName.sprintf("/archon1_%dx%d_%d_1000ms.raw", lastFrameStatus[2], lastFrameStatus[3], lastFrameStatus[0]));
    int bytesRemaining = frameSize;

    if (!imgFile.open(QFile::WriteOnly|QFile::Truncate)) {
        QMessageBox::warning(this, "Error", "Cannot open write file!");

        ui->btnFetch_1->setText("Fetch");

        return;
    }

    for (int i = 0; i < lines; i++) {
        archon_1->minusOneMsgRef();
        imgFile.write(archon_1->archonBinRecv().left(qMin(lineSize, bytesRemaining)).toStdString().c_str());
        bytesRemaining -= lineSize;
    }

    imgFile.close();

    archon_1->plusOneMsgRef();

    // Read status
    QString response = archon_1->archonCmd("STATUS");

    if (response == "") { return; }

    QStringList pairs = response.split(' ');

    for (auto pair : pairs) {
        QString str;
        QStringList keyValue = pair.split('=');

        if (keyValue[0] == "MOD10/TEMP") {
            ui->lbTemp_1->setText(str.sprintf("MODm/TEMPA = %s (C)",keyValue[1].trimmed().toStdString().c_str()));
        }
        else if (keyValue[0] == "MOD10/TEMPA") {
            ui->lbSensorATemp_1->setText(str.sprintf("MODm/TEMPA = %s (K)",keyValue[1].trimmed().toStdString().c_str()));
        }
        else if (keyValue[0] == "MODm/TEMPB") {
            ui->lbSensorBTemp_1->setText(str.sprintf("MODm/TEMPB = %s (K)",keyValue[1].trimmed().toStdString().c_str()));
        }
        else if (keyValue[0] == "MODm/TEMPC") {
            ui->lbSensorCTemp_1->setText(str.sprintf("MODm/TEMPC = %s (K)",keyValue[1].trimmed().toStdString().c_str()));
        }
    }

    // raw2fits
    QFile rawFile(saveDirectoryPath + fileName);

    if (!rawFile.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Cannot open raw file!");

        return;
    }

    QFileInfo fileInfo(rawFile.fileName());
    int w = fileInfo.fileName().split("_")[1].split("x")[0].toInt();
    int h = fileInfo.fileName().split("_")[1].split("x")[1].toInt();

    saveFITS(rawFile, w, h);

    ui->btnFetch_1->setText("Complete");
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
