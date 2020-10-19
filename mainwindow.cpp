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
