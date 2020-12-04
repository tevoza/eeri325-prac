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
#include <fftw3.h>

#define REAL 0
#define IMAG 1

class mySignalProcessing
{
public:
    mySignalProcessing();
};

using namespace std;
vector<complex<double>> MyFFT(vector<complex<double>> &samples);
vector<complex<double>> MyInvFFT(vector<complex<double>> &samples);
QVector<double> myMagSpectrum(vector<complex<double>> &freqBins);
QVector<double> myPhaseSpectrum(vector<complex<double>> &freqBins);
QVector<double> My4kLPF(QVector<double> &samples);
QVector<double> My4kHPF(QVector<double> &samples);
void ftw_test_1d(int N, fftw_complex *input, fftw_complex *output);
void ftw_run();

#endif // MYSIGNALPROCESSING_H
