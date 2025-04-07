#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdint.h>
using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

int main(){
    int nr_potions;
    fin >> nr_potions;

    vector<int64_t> gandalf_pot(nr_potions),
                merlin_pot(nr_potions);
    
    for (auto& pot : gandalf_pot)
        fin >> pot;
    
    for (auto& pot : merlin_pot)
        fin >> pot;
    
    uint64_t res = 0;
    for (int i = 0; i < nr_potions - 1; ++i)
        for (int j = i + 1; j < nr_potions; ++j){
            if ((gandalf_pot[i] + gandalf_pot[j]) > (merlin_pot[i] + merlin_pot[j])){
                // fout << i << " " << j << '\n';
                res ++;
            }
            else if ((gandalf_pot[i] + gandalf_pot[j]) < (merlin_pot[i] + merlin_pot[j])){
                res--;
            }
        }

    fout << res % ((int)1e9 + 7);
}
