#include <iostream>
#include <vector>
#include <complex>

using namespace std;

int main()
{
   //vector<complex<double>> 
    complex<double> num1(0.0 , -1.0);

    cout << real(num1) << " + j" << imag(num1) << endl;
    cout << polar(real(num1), imag(num1)) << endl;

    return 0;
}

//return a vector of frequency bins
vector<complex<double>> myfft(vector<complex<double>> &samples)
{
    int N = samples.size();

    
    if(N == 1){
        return samples;
    }

}
