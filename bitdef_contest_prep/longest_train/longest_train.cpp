#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <numeric>
#include <stdint.h>
#include <queue>
using namespace std;

//100p
ifstream fin("data.in");
ofstream fout("data.out");

int main(){
    uint64_t nr_trains;
    fin >> nr_trains;

    vector<uint64_t> trains(nr_trains);
    for (int i = 0; i < nr_trains; ++i)
        fin >> trains[i];
    
    priority_queue<uint64_t, vector<uint64_t>, greater<uint64_t>> pq(trains.begin(), trains.end());

    uint64_t res = 0;
    while(pq.size() > 1){
        uint64_t min1 = pq.top();
        pq.pop();
        uint64_t min2 = pq.top();
        pq.pop();
        uint64_t new_train = min1 + min2;

        res += new_train;
        pq.push(new_train);
    }

    fout << res;
}