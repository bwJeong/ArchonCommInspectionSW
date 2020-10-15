#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    archon_1 = new Archon();
    archon_2 = new Archon();
    archon_3 = new Archon();
}

MainWindow::~MainWindow() {
    delete ui;
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

    archon_1->configClear();

    if (!rFile.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open config file!");
    }

    while (!rFile.atEnd()) {
        QString line = rFile.readLine();

        if (sectionTrigger == true) {
            int pos = line.indexOf('=');
            QString key = line.left(pos).toUpper().replace("\\", "/");
            QString value = line.remove(0, pos + 1).replace("\"", "").trimmed();

            archon_1->configPush_back(key, value);
        }

        if (line == "[CONFIG]\n") { sectionTrigger = true; }
    }

    rFile.close();

    ui->leConfigFilePath_1->setText(fileName);
}

void MainWindow::on_btnApplyConfig_1_clicked() {
    QString dateNTime;

    // Send "CLEARCONFIG" command & Receive ack
    if (archon_1->archonSend("CLEARCONFIG") != -1) {
        dateNTime = QDateTime::currentDateTime().toString("yy.MM.dd / hh:mm:ss - ");

        ui->lwTx_1->addItem(dateNTime + "CLEARCONFIG");

        QString rBuf = archon_1->archonRecv();

        if (rBuf != "") {
            QString ackFormat;
            QString ack = rBuf.left(3);

            if (ack == ackFormat.sprintf("<%02X", archon_1->plusOneMsgRef())) {
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

    for (int i = 0; i < archon_1->configSize(); i++) {
        configCommand.sprintf("WCONFIG%04X%s=%s", i, archon_1->configKey_at(i).toStdString().c_str(), archon_1->configValue_at(i).toStdString().c_str());

        if (archon_1->archonSend(configCommand) != -1) {
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
