#include <iostream>
#include <vector>
#include <complex>
#include <math.h>

using namespace std;

complex<double> myfftbin(int k, vector<complex<double>> &samples);

int main()
{
    vector<complex<double>> samples; 
    vector<complex<double>> bins;
    cout << "samples:" << endl;
    int sampleFreq = 4;

    complex<double> sample;
    for (int i = 0; i < 8; i++){
        sample = complex<double>(sin(1.5*i*(M_PI/2)), 0.0);
        samples.push_back(sample);
        cout << real(samples[i]) << "+j(" << imag(samples[i]) << "), ";
    }
    cout << endl;

    double binHz;
    for (int i = 0; i < samples.size(); i++){
        bins.push_back(myfftbin(i, samples));
        binHz = i*(double(sampleFreq)/double(samples.size()));
        cout << i << "th bin(" << binHz << ") -> "<< bins[i] << endl;
    }
    cout << endl;

    return 0;
}

//return a vector of frequency bins
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
