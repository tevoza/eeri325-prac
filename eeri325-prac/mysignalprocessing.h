#ifndef MYSIGNALPROCESSING_H
#define MYSIGNALPROCESSING_H

#include <complex.h>
#include <math.h>
#include <QVector>
#include <QMainWindow>
#include <QDirIterator>
#include <QSound>
#include <QAudioDecoder>
#include <QAudioDeviceInfo>
#include <QtGlobal>


class mySignalProcessing
{
public:
    mySignalProcessing();
};

using namespace std;
vector<complex<double>> MyFFT(vector<complex<double>> &samples);
QVector<double> My4kLPF(QVector<double> &samples);
complex<double> myfftbin(int k, vector<complex<double>> &samples);
vector<complex<double>> myfft(vector<complex<double>> &samples);
QVector<double> myMagSpectrum(vector<complex<double>> &freqBins);


#endif // MYSIGNALPROCESSING_H
