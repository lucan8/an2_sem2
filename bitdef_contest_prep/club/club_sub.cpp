#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <queue>
#include <unordered_set>
using namespace std;

bool canReach(int max_value, int start, int end, const vector<int>& node_costs,
              const vector<vector<int>>& neighbours){
    if (node_costs[start] > max_value)
        return false;
    queue<int> q;
    vector<bool> visited(node_costs.size(), false);

    q.push(start);

    while (!q.empty()){
        int curr = q.front();
        q.pop();

        if (curr == end) return true;

        for (int neigh : neighbours[curr])
            if (!visited[neigh] && node_costs[neigh] <= max_value){
                q.push(neigh);
                visited[curr] = true;
            }
    }
    return false;
}

//Doar 4 teste
int main(){
    int nr_nodes, nr_edges, start, end;
    cin >> nr_nodes >> nr_edges >> start >> end;

    int min_cost = 1e10, max_cost = 0;
    vector<int> node_costs(nr_nodes + 1);

    for (int i = 1; i <= nr_nodes; ++i){
        cin >> node_costs[i];
        min_cost = min(min_cost, node_costs[i]);
        max_cost = max(max_cost, node_costs[i]);
    }
    
    vector<vector<int>> neighbours(nr_nodes + 1);
    for (int i = 0; i < nr_edges; ++i){
        int n1, n2;
        cin >> n1 >> n2;
        
        neighbours[n1].push_back(n2);
        neighbours[n2].push_back(n1);
    }

    int curr_best = -1;
    while (min_cost <= max_cost){
        int mid = min_cost + (max_cost - min_cost) / 2;

        if (canReach(mid, start, end, node_costs, neighbours)){
            curr_best = mid;
            max_cost = mid - 1;
        }
        else
            min_cost = mid + 1;
    }

    cout << curr_best;
}