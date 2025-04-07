#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <queue>
#include <unordered_set>
#include <algorithm>
using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

const int TABLE_SIZE = 5;
const int MOD_NR = 999999937;

int main(){
    uint64_t nr_rotations;
    // cin >> nr_rotations;

    vector<uint64_t> table(TABLE_SIZE, 1);
    
    vector<uint64_t> big_table(100);
    // for (uint64_t i = 0; i < nr_rotations; ++i)
    //     table[i % TABLE_SIZE] = (table[(i + 1) % TABLE_SIZE] + table[(i + 4) % TABLE_SIZE]) % MOD_NR;

    // cout << table[nr_rotations % 5];
    for (uint64_t i = 0; i < big_table.size(); ++i){
        big_table[i] = table[i % TABLE_SIZE] % MOD_NR;
        table[i % TABLE_SIZE] = (table[(i + 1) % TABLE_SIZE] + table[(i + 4) % TABLE_SIZE]) % MOD_NR;
    }

    for (int i = 0; i < big_table.size(); i+=5)
        cout << big_table[i] << "\n";
    
}