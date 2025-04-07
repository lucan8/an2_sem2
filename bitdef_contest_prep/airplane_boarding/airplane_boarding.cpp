#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

int backt(int val, int curr_i, int curr_j, const vector<int>& pass_tickets, int nr_pass){
    if (curr_i >= curr_j)
        return nr_pass;

    int v1 = nr_pass, v2 = nr_pass, v3 = nr_pass;
    if (val == pass_tickets[curr_i] + pass_tickets[curr_i + 1])
        v1 = backt(val, curr_i + 2, curr_j, pass_tickets, nr_pass + 1);
    if (val == pass_tickets[curr_i] + pass_tickets[curr_j])
        v2 = backt(val, curr_i + 1, curr_j - 1, pass_tickets, nr_pass + 1);
    if (val == pass_tickets[curr_j] + pass_tickets[curr_j - 1])
        v3 = backt(val, curr_i, curr_j - 2, pass_tickets, nr_pass + 1);
    
    return max(max(v1, v2), v3);
}

int main(){
    int nr_pass;
    fin >> nr_pass;

    vector<int> pass_tickets(nr_pass);

    for (auto& ticket : pass_tickets)
        fin >> ticket;
    
    int curr_i = 0, curr_j = nr_pass - 1;
    int val1 = pass_tickets[curr_i] + pass_tickets[curr_i + 1];
    int val2 = pass_tickets[curr_i] + pass_tickets[curr_j];
    int val3 = pass_tickets[curr_j] + pass_tickets[curr_j - 1];

    int res = max(
                    max(
                        backt(val1, curr_i + 2, curr_j, pass_tickets, 1),
                        backt(val2, curr_i + 1, curr_j - 1, pass_tickets, 1)
                       ),
                    backt(val3, curr_i, curr_j - 2, pass_tickets, 1)
                 );
    fout << res * 2;
}