#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <queue>
#include <unordered_set>
using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

//Doar 4 teste

int main(){
    int nr_nodes, nr_edges, start, end;
    fin >> nr_nodes >> nr_edges >> start >> end;

    vector<uint32_t> node_costs(nr_nodes + 1);
    for (int i = 1; i <= nr_nodes; ++i)
        fin >> node_costs[i];
    
    vector<vector<int>> neighbours(nr_nodes + 1);
    for (int i = 0; i < nr_edges; ++i){
        int n1, n2;
        fin >> n1 >> n2;
        
        neighbours[n1].push_back(n2);
        neighbours[n2].push_back(n1);
    }

    auto comp = [node_costs](int n1, int n2){
        return node_costs[n1] > node_costs[n2];
    };

    priority_queue<int, vector<int>, decltype(comp)> pq(comp);
    vector<bool> visited(nr_nodes + 1, false);

    visited[start] = true;
    pq.push(start);

    while (!pq.empty()){
        int curr = pq.top();
        pq.pop();

        for (int neigh : neighbours[curr])
            if (!visited[neigh]){
                visited[neigh] = true;
                node_costs[neigh] = max(node_costs[curr], node_costs[neigh]);
                pq.push(neigh);
            }
    }

    if (!visited[end])
        fout << -1;
    else
        fout << node_costs[end];
}