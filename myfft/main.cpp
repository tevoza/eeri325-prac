#include <iostream>
#include <vector>
#include <complex>

int main()
{
   //vector<complex<double>> 

    std::cout << "HI!\n";
    return 0;

}

//return a vector of frequency bins
std::vector<std::complex<double>> myfft(std::vector<std::complex<double>> &samples)
{
    int N = samples.size();

    
    if(N == 1){
        return samples;
    }

}
