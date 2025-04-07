#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>

using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

int main(){
    vector<vector<uint64_t>> dp(26, vector<uint64_t>(26, 0));

    for (int i = 1; i < dp.size(); ++i){
        for (int j = 1; j < dp.size(); ++j){
            if (i == 1){
                dp[i][j] = j;
                continue;
            }
            if (j < i)
                dp[i][j] = dp[i - 1][j];
            else
                dp[i][j] = dp[i - 1][j] + dp[i][j - 1] + (i == j);
        }
    }

    int nr_q;
    fin >> nr_q;
    for (int i = 0; i < nr_q; ++i){
        int nr_basckets, nr_balls;
        fin >> nr_basckets >> nr_balls;
        
        cout << dp[nr_balls][nr_basckets] << '\n';
    }
}