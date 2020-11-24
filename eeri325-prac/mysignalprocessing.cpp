#include "mysignalprocessing.h"

mySignalProcessing::mySignalProcessing()
{

}

//return a vector of frequency bins
vector<complex<double>> MyFFT(vector<complex<double>> &samples)
{
    int N = samples.size();

    if (N == 1) { return samples;}  //return self at bottom of recursion

    //split into even and odd sub samples
    int M = N/2;
    vector<complex<double>> xEven(M,0);
    vector<complex<double>> xOdd(M,0);
    for(int i =0; i<M; i++){
        xEven[i] = samples[2*i];
        xOdd[i] = samples[2*i+1];
    }

    //recursive call on smaller sets
    vector<complex<double>> FEven(M,0);
    vector<complex<double>> FOdd(M,0);
    FEven = MyFFT(xEven);
    FOdd = MyFFT(xOdd);

    //End of recursion, now combine
    vector<complex<double>> freqBins(N,0);
    complex<double> oddcmp;
    for(int k=0 ; k < N/2; k++)
    {
        oddcmp = polar(1.0, (-2*M_PI*k)/N)*FOdd[k];
        freqBins[k] = FEven[k] + oddcmp;
        freqBins[k+N/2] = FEven[k] - oddcmp;
    }

    return freqBins;
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

QVector<double> My4kLPF(QVector<double> &samples)
{
    QVector<double> Filtered(samples.size());
    for (int i=2; i<samples.size(); i++)
    {
        Filtered[i] = 0.17724*samples[i] + 0.35449*samples[i-1] + 0.17724*samples[i-2] + 0.50872*Filtered[i-1] - 0.2177*Filtered[i-2];
    }
    return Filtered;
}
