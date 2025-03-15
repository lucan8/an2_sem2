#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

double f(double x, int a, int b, int c){
    return a * x * x + b * x + c;
}

int main(){
    ifstream fin("date.in");
    ofstream fout("date.out");

    int a, b, c, nr_chromo;
    cin >> a >> b >> c >> nr_chromo;

    vector<double> chromo(nr_chromo);
    vector<double> fitness_chromo(nr_chromo);
    double fitness = 0;

    for (int i = 0; i < nr_chromo; ++i){
        cin >> chromo[i];
        fitness_chromo[i] = f(chromo[i], a, b, c);
        fitness += fitness_chromo[i];
    }

    double partial_sum = 0;
    for (auto x : fitness_chromo){
        cout << partial_sum / fitness << "\n";
        partial_sum += x;
    }

    cout << partial_sum / fitness << "\n";
}