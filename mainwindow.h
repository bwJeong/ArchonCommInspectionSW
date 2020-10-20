#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>
#include <QVector>
#include <QTimer>
#include <QtEndian>
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

    //
    void makeSaveDirectory();

    void makeTxLogSaveFile_1();
    void makeRxLogSaveFile_1();
    void txLogAutoSave_1(QString txStr);
    void rxLogAutoSave_1(QString rxStr);
    void closeTxLogSaveFile_1();
    void closeRxLogSaveFile_1();

    // raw2fits
    void addFITSHeader(QFile &fitsFile, QString key, QString value, QString comment);
    void endFITSHeader(QFile &fitsFile, int lines);
    void saveFITS(QFile &rawFile, const int w, const int h);

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

    void on_btnTgTxLogAutoSave_1_toggled(bool checked);

    void on_btnTgRxLogAutoSave_1_toggled(bool checked);

private:
    Ui::MainWindow *ui;   
    Archon *archon_1, *archon_2, *archon_3;
    QFile *txLogSaveFile_1, *rxLogSaveFile_1;

    QVector<QVector<QString>> sections_1, sections_2, sections_3;
    QVector<QString> configKeys_1, configValues_1;
    QVector<QString> configKeys_2, configValues_2;
    QVector<QString> configKeys_3, configValues_3;
    bool isTxLogSaveFileCreated_1, isRxLogSaveFileCreated_1;
    QString hourCheck;
};

#endif // MAINWINDOW_H
