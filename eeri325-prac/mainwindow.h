#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDirIterator>
#include <QSound>
#include <QAudioDecoder>
#include <QAudioDeviceInfo>
#include <QMediaResource>
#include <QtGlobal>
#include <QPoint>
#include <QColor>
#include "mysignalprocessing.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSound *orgSound;
    QAudioDecoder *decoder;
    QVector <double> orgSoundSignal;
    QVector <double> filteredSoundSignal;
    vector <complex<double>> cmpOrgSoundSignal;
    vector <complex<double>> cmpfilteredSignal;
    double sampleFreq;

    //images
    QImage *InputImage;

    vector<complex<double>> toComplex(QVector<double> &signal);

    public slots:
    void updateSoundFiles();
    void playOrgSound();
    void decodeOrgSound();
    void readBuffer();
    void plotOrgTimePlot();
    void plotFilteredSignal();
    void testfft();
    void filfft();

private slots:
    void on_orgBtnPlot_clicked();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
