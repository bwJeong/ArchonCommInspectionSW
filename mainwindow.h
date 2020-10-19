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

signals:
    void configFileReadError();

public slots:
    void readConfig(QFile &rFile);

private slots:
    void on_btnReadConfig_1_clicked();

    void on_btnApplyConfig_1_clicked();

    void on_btnTgConnection_1_toggled(bool checked);

private:
    Ui::MainWindow *ui;   
    Archon *archon_1, *archon_2, *archon_3;
    QVector<QVector<QString>> sections_1, sections_2, sections_3;
    QVector<QString> configKeys_1, configValues_1;
    QVector<QString> configKeys_2, configValues_2;
    QVector<QString> configKeys_3, configValues_3;
};

#endif // MAINWINDOW_H
