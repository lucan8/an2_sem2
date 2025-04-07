#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

int solver(int value, int start, int end, const vector<int>& pass_tickets){
    int nr_pass = pass_tickets.size();
    // dp[i][j] = max nr of passangers from i->j
    vector<vector<int>> dp(nr_pass, vector<int>(nr_pass));

    // Only the passangers that represent arg "value" are chosen
    if (start > end)
        return 2;
    
    for (int i = start; i <= end; ++i)
        dp[i][i] = pass_tickets[i] == value;
    
    for (int i = end - 1; i >= start; --i)
        for (int j = i + 1; j <= end; ++j){
            int val1 = 0, val2 = 0, val3 = 0;
            if (pass_tickets[i] + pass_tickets[j] == value)
                val1 = 2 + dp[i + 1][j - 1];
            else if (pass_tickets[i] + pass_tickets[i + 1] == value)
                val2 = 2 + dp[i + 2][j];
            else if (pass_tickets[j] + pass_tickets[j - 1] == value)
                val3 = 2 + dp[i][j - 2];
            dp[i][j] = max(val1, max(val2, val3));
        }
    return dp[start][end] + 2;        
}
int main(){
    int nr_pass;
    cin >> nr_pass;

    vector<int> pass_tickets(nr_pass);

    for (auto& ticket : pass_tickets)
        cin >> ticket;
    
    int val1 = pass_tickets[0] + pass_tickets[1];
    int val2 = pass_tickets[0] + pass_tickets[nr_pass - 1];
    int val3 = pass_tickets[nr_pass - 1] + pass_tickets[nr_pass - 2];

    cout << max(
                max(
                    solver(val1, 2, nr_pass - 1, pass_tickets),
                    solver(val2, 1, nr_pass - 2, pass_tickets)
                   ),
                solver(val3, 0, nr_pass - 3, pass_tickets)
                );
}