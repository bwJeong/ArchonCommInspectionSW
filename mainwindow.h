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

    void readConfig(QFile &rFile, QVector<QVector<QString>> &sections, QVector<QString> &configKeys, QVector<QString> &configValues);

    // Log file
    void makeTxLogSaveFile_1();
    void makeTxLogSaveFile_2();
    void makeTxLogSaveFile_3();

    void makeRxLogSaveFile_1();
    void makeRxLogSaveFile_2();
    void makeRxLogSaveFile_3();

    void txLogAutoSave_1(QString txStr);
    void txLogAutoSave_2(QString txStr);
    void txLogAutoSave_3(QString txStr);

    void rxLogAutoSave_1(QString rxStr);
    void rxLogAutoSave_2(QString rxStr);
    void rxLogAutoSave_3(QString rxStr);

    void closeTxLogSaveFile_1();
    void closeTxLogSaveFile_2();
    void closeTxLogSaveFile_3();

    void closeRxLogSaveFile_1();
    void closeRxLogSaveFile_2();
    void closeRxLogSaveFile_3();

    // raw2fits
    void addFITSHeader(QFile &fitsFile, QString key, QString value, QString comment);
    void endFITSHeader(QFile &fitsFile, int lines);
    void saveFITS(QFile &rawFile, const int w, const int h, QVector<QString> &statusKeys, QVector<QString> &statusValues);

private slots:
    void checkFrameStatusChange_1();
    void checkFrameStatusChange_2();

    void archonSignalResponse_1(int num, QString str);
    void archonSignalResponse_2(int num, QString str);
    void archonSignalResponse_3(int num, QString str);

    void processEvent();

    // Btn
    void on_btnTgConnection_1_toggled(bool checked);
    void on_btnTgConnection_2_toggled(bool checked);

    void on_btnReadConfig_1_clicked();
    void on_btnReadConfig_2_clicked();

    void on_btnApplyConfig_1_clicked();
    void on_btnApplyConfig_2_clicked();

    void on_btnCcdPwrOn_1_clicked();
    void on_btnCcdPwrOn_2_clicked();
    void on_btnCcdPwrOff_1_clicked();
    void on_btnCcdPwrOff_2_clicked();

    void on_btnExpose_1_clicked();
    void on_btnExpose_2_clicked();

    void on_btnFetch_1_clicked();
    void on_btnFetch_2_clicked();

    void on_btnTgTxLogAutoSave_1_toggled(bool checked);
    void on_btnTgTxLogAutoSave_2_toggled(bool checked);
    void on_btnTgTxLogAutoSave_3_toggled(bool checked);

    void on_btnTgRxLogAutoSave_1_toggled(bool checked);
    void on_btnTgRxLogAutoSave_2_toggled(bool checked);
    void on_btnTgRxLogAutoSave_3_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    Archon *archon_1, *archon_2, *archon_3;
    QFile *txLogSaveFile_1, *rxLogSaveFile_1;
    QFile *txLogSaveFile_2, *rxLogSaveFile_2;
    QFile *txLogSaveFile_3, *rxLogSaveFile_3;
    QTimer *frameStatusCheckTimer_1, *frameStatusCheckTimer_2;

    QVector<QVector<QString>> sections_1, sections_2;

    QVector<QString> configKeys_1, configValues_1;
    QVector<QString> configKeys_2, configValues_2;

    QVector<QString> statusKeys_1, statusValues_1;
    QVector<QString> statusKeys_2, statusValues_2;
    QVector<QString> statusKeys_3, statusValues_3;

    bool isTxLogSaveFileCreated_1, isRxLogSaveFileCreated_1;
    bool isTxLogSaveFileCreated_2, isRxLogSaveFileCreated_2;
    bool isTxLogSaveFileCreated_3, isRxLogSaveFileCreated_3;

    QString hourCheck;
    QString fitsDateTime;
};

#endif // MAINWINDOW_H
