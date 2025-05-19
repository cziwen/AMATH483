#include <iostream>
#include <vector>
#include <thread>
#include <numeric>
#include <cmath>
#include <chrono>
#include <iomanip>  
using namespace std;

double fprime(double x){
    return 1.0/x - x/4.0;
}
double integrand(double x){
    return sqrt(1 + fprime(x)*fprime(x));
}

int main(int argc,char** argv){
    if(argc<3){
        cerr<<"Usage: "<<argv[0]<<" N_points N_threads\n";
        return 1;
    }
    long long N = atoll(argv[1]);
    int T     = atoi(argv[2]);
    double a=1.0, b=6.0, h=(b-a)/N;
    vector<double> partial(T,0.0);

    auto worker = [&](int id){
        long long chunk = N/T;
        long long start = id*chunk;
        long long end   = (id==T-1 ? N : start+chunk);
        double sum=0;
        for(long long i=start;i<end;i++){
            double x = a + (i+0.5)*h;
            sum += integrand(x);
        }
        partial[id] = sum;
    };

    // 计时开始
    auto t0 = chrono::high_resolution_clock::now();
    vector<thread> threads;
    for(int t=0;t<T;t++) threads.emplace_back(worker,t);
    for(auto& th:threads) th.join();
    double total = accumulate(partial.begin(), partial.end(), 0.0) * h;
    auto t1 = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration<double>(t1-t0).count();

    // 准确值
    const double exact = 6.16676;
    double error = fabs(total - exact);
    double log_error = log10(error);

    cout<<fixed<<setprecision(8);
    cout<<"Length ≈ " << total << "\n";
    cout<<"Time   = " << elapsed << " s\n";
    cout<<"Error  = " << error << "\n";
    cout<<"log10(error) = " << log_error << "\n";

    return 0;
}
