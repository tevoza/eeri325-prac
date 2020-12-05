#include "mysignalprocessing.h"

mySignalProcessing::mySignalProcessing()
{

}

vector<complex<double> > MyFFT(vector<complex<double> > &samples)
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


QVector<double> myMagSpectrum(vector<complex<double> > &freqBins)
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

void ftw_test_1d(int N, fftw_complex *input, fftw_complex *output)
{
    fftw_plan my_plan = fftw_plan_dft_1d(N, input, output, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(my_plan);

    fftw_destroy_plan(my_plan);
    fftw_cleanup();
    return;
}

void ftw_run()
{
    int N = 8;
    fftw_complex in[N];
    fftw_complex out[N];

    for(int i = 0; i < N; i++){
        in[i][REAL] = sin(2.0*(M_PI/4));
        in[i][IMAG] = 0;
    }

   ftw_test_1d(N, in, out);

   for(int i = 0; i < N; i++){
       qDebug() << out[i][REAL] << " +j" << out[i][IMAG];
   }

}

QVector<double> myPhaseSpectrum(vector<complex<double>> &freqBins)
{
    QVector<double> PhaseSpec;
    for (unsigned int k =0; k < freqBins.size(); k++)
    {
        PhaseSpec.push_back(arg(freqBins[k]));
    }
    return PhaseSpec;
}

QVector<double> My4kHPF(QVector<double> &samples)
{
    QVector<double> Filtered(samples.size());
    for (int i=2; i<samples.size(); i++)
    {
        Filtered[i] = -0.17724*samples[i] + 0.35449*samples[i-1] - 0.17724*samples[i-2] + 0.50872*Filtered[i-1] + 0.2177*Filtered[i-2];
    }
    return Filtered;

}

vector<complex<double> > MyInvFFT(vector<complex<double> > &samples)
{
    int N = samples.size();
    vector<complex<double>> con;
    vector<complex<double>> res;

    //find the samples conjugate
    for(int i = 0; i < samples.size(); i++)
        con.push_back(conj(samples[i]));

    res = MyFFT(con);

    for(int i = 0; i < res.size(); i++)
    {
        res[i] = (conj(res[i]))/double(N);
    }

    return res;
}
