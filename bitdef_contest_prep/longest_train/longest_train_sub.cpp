#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <numeric>
#include <stdint.h>
#include <queue>
using namespace std;


int main(){
    int nr_trains;
    cin >> nr_trains;

    vector<uint64_t> trains(nr_trains);
    for (uint64_t i = 0; i < nr_trains; ++i)
        cin >> trains[i];
    
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

    cout << res;
}