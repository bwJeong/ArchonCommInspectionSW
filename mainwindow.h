#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QVector>
#include <QDebug>
#include "archon.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private slots:
    void on_btnReadConfig_1_clicked();

    void on_btnApplyConfig_1_clicked();

private:
    Ui::MainWindow *ui;   
    Archon *archon_1, *archon_2, *archon_3;
};

#endif // MAINWINDOW_H
