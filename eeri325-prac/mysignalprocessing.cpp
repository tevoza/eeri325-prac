#include "mysignalprocessing.h"

mySignalProcessing::mySignalProcessing()
{

}

//return the k-th freq bin
complex<double> myfftbin(int k, vector<complex<double>> &samples)
{
    int N = samples.size();

    //return self for bottom level recursion
    if(N == 1){
        complex<double> cmp(1.0, 0.0);
        return cmp*samples[0];
    }

    //split even and odd components
    vector<complex<double>> evens;
    vector<complex<double>> odds;
    for (int i = 0; i<samples.size(); i++){
        if (i % 2 == 0){
            evens.push_back(samples[i]);
        } else {
            odds.push_back(samples[i]);
        }
    }

    //recursive call on smaller sets
    complex<double> oddcmp = polar(1.0, (-2*M_PI*k)/N);
    complex<double> res = myfftbin(k, evens) + oddcmp*myfftbin(k, odds);

    return res;
}

//return a vector of frequency bins
vector<complex<double>> myfft(vector<complex<double>> &samples)
{
    vector<complex<double>> bins;
    for(unsigned int k = 0; k<samples.size(); k++){
        bins.push_back(myfftbin(k, samples));
    }
    return bins;
}

QVector<double> myMagSpectrum(vector<complex<double>> &freqBins)
{
    QVector<double> MagSpec;
    for (unsigned int k = 0; k<freqBins.size(); k++)
    {
        MagSpec.push_back(abs(freqBins[k]));
    }
    return MagSpec;
}
