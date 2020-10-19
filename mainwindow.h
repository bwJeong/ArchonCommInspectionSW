#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QVector>
#include <QTimer>
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

    void readConfig(QFile &rFile);

signals:

public slots:
    void checkFrameStatusChange_1();
    void archonSignalResponse_1(int num, QString str);
    void processEvent();

private slots:
    void on_btnReadConfig_1_clicked();

    void on_btnApplyConfig_1_clicked();

    void on_btnTgConnection_1_toggled(bool checked);

    void on_btnExpose_1_clicked();

    void on_btnFetch_1_clicked();

private:
    Ui::MainWindow *ui;   
    Archon *archon_1, *archon_2, *archon_3;
    QVector<QVector<QString>> sections_1, sections_2, sections_3;
    QVector<QString> configKeys_1, configValues_1;
    QVector<QString> configKeys_2, configValues_2;
    QVector<QString> configKeys_3, configValues_3;
    QTimer *currentFrameCheckTimer;
};

#endif // MAINWINDOW_H
