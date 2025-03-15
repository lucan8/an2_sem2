#include <iostream>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <vector>
using namespace std;

string toBits(int n, int nr_bits){
    string res = "";
    while (n){
        res += to_string(n % 2);
        n = n >> 1;
    }

    while (res.size() != nr_bits)
        res.push_back('0');

    reverse(res.begin(), res.end());

    return res;
}

int fromBits(const string& bits){
    int res = 0;
    for (int i = 0; i < bits.size(); ++i)
        res += (bits[i] - 48) << bits.size() - i - 1;
    return res;
}

int main(){
    ifstream fin("date.in");
    ofstream fout("date.out");
    int p, nr_queries, nr_bits;
    float a, b, interval_step;

    string instr;

    cin >> a >> b >> p >> nr_queries;

    nr_bits = log2((b - a) * pow(10, p)) + 1;
    interval_step = (b - a) / (1 << nr_bits);

    while (nr_queries--){
        cin >> instr;
        if (instr == "TO"){
            double nr;
            int interval_index;
            cin >> nr;
        
            interval_index = (int)((nr - a) / interval_step);
            string bits = toBits(interval_index, nr_bits);
            cout << bits << "\n";
        }
        else if (instr == "FROM"){
            string bits;
            cin >> bits;

            int nr = fromBits(bits);
            cout << a + nr * interval_step << '\n';
        }
    }
}