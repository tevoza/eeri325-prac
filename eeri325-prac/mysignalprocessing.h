#ifndef MYSIGNALPROCESSING_H
#define MYSIGNALPROCESSING_H

#include <complex.h>
#include <math.h>
#include <QVector>

class mySignalProcessing
{
public:
    mySignalProcessing();
};

using namespace std;
complex<double> myfftbin(int k, vector<complex<double>> &samples);
vector<complex<double>> myfft(vector<complex<double>> &samples);
QVector<double> myMagSpectrum(vector<complex<double>> &freqBins);


#endif // MYSIGNALPROCESSING_H
