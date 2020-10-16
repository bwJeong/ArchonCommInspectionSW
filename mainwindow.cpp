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

    if (fileName == "") { return; }

    QFile rFile(fileName);

    // Read Config file

}

void MainWindow::on_btnApplyConfig_1_clicked() {

}
