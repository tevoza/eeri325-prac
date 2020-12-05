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
#include <fftw3.h>

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

    double sampleFreq;
    //original signal
    QVector <double> orgSoundSignal;
    vector <complex<double>> cmpOrgSoundSignal;
    vector <complex<double>> bins; //orgininal fft output


    //filtered signal
    QVector <double> filteredSoundSignal;
    vector <complex<double>> cmpfilteredSignal;
    vector <complex<double>> filteredfft;
    vector <complex<double>> invfft;

    //images
    QImage *InputImage;
    vector<vector<int>> vecInputImage;
    vector<vector<int>> saveImage(QImage &Image);
    QImage setImage(vector<vector<int> > &ImgVec);

    vector<complex<double>> toComplex(QVector<double> &signal);


    vector<vector<int>> saveImage(QImage *Image);
    QImage *setImage(vector<vector<int>> &ImgVec);

    public slots:
    void process2d();
    void processImage();
    void updateSoundFiles();
    void playOrgSound();
    void decodeOrgSound();
    void readBuffer();
    //original signal
    void plotOrgTimePlot();
    void plotOrgMagSpec();
    void plotOrgPhaseSpec();
    void testfft();

    //filtered signal
    void plotFilteredSignal();
    void plotFilteredMag();
    void plotFilteredPhase();
    void plotInv();
    void filfft();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
