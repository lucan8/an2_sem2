#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int MOD = 1e9 + 7;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int n;
    cin >> n;
    
    vector<int> g(n), m(n), d(n);
    for (int i = 0; i < n; ++i) cin >> g[i];
    for (int i = 0; i < n; ++i) cin >> m[i];
    for (int i = 0; i < n; ++i) d[i] = g[i] - m[i];
    
    sort(d.begin(), d.end());
    
    long long res = 0;
    int left = 0, right = n - 1;
    
    while (left < right) {
        if (d[left] + d[right] > 0) {
            // All pairs with this right element are valid
            res += right - left;
            right--;
        } else {
            left++;
        }
    }
    
    cout << res % MOD << '\n';
    return 0;
}