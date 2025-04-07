#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

int binS(int val, int start, int end, const vector<int>& pot_diff){
    int res = pot_diff.size();

    while (start <= end){
        int mid = start + (end - start) / 2;

        if (pot_diff[mid] + val >= 1){
            res = mid;
            end = mid - 1;
        }
        else
            start = mid + 1;
    }
    return res;
}

int main(){
    int nr_potions;
    fin >> nr_potions;

    vector<int> gandalf_pot(nr_potions),
                merlin_pot(nr_potions);
    
    for (auto& pot : gandalf_pot)
        fin >> pot;
    
    for (auto& pot : merlin_pot)
        fin >> pot;
    
    vector<int> pot_diff(nr_potions);
    for (int i = 0; i < nr_potions; ++i)
        pot_diff[i] = gandalf_pot[i] - merlin_pot[i];
    
    sort(pot_diff.begin(), pot_diff.end());

    uint64_t res = 0;
    for (int i = 0; i < nr_potions - 1; ++i){
        int start;

        if (pot_diff[i] > 0)
            start = i + 1;
        else
            start = binS(pot_diff[i], i + 1, pot_diff.size() - 1, pot_diff);

        fout << start << '\n';
        res += nr_potions - start;
    }

    fout << res % ((int)1e9 + 7);
}