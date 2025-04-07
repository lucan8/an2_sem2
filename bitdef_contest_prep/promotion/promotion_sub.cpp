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

struct Task{
    int andrei_score;
    int bogdan_score;
};

int main(){
    int nr_tasks;
    cin >> nr_tasks;

    vector<Task> tasks(nr_tasks);
    if (nr_tasks < 100000)
        cout << "Got you\n";
    else
        while(true);
    
    for (int i = 0; i < nr_tasks; ++i)
        cin >> tasks[i].andrei_score;

    for (int i = 0; i < nr_tasks; ++i)
        cin >> tasks[i].bogdan_score;
    
    // Sort desc by maximum score and by sum on equality
    auto task_comp = [](const Task& t1, const Task& t2){
        if (max(t1.andrei_score, t1.bogdan_score) == max(t2.andrei_score, t2.bogdan_score))
            return abs(t1.andrei_score - t1.bogdan_score) < abs(t2.andrei_score - t2.bogdan_score);
        return max(t1.andrei_score, t1.bogdan_score) > max(t2.andrei_score, t2.bogdan_score);
    };

    sort(tasks.begin(), tasks.end(), task_comp);

    int64_t res = 0;
    for (int i = 0; i < tasks.size(); ++i)
        if (i % 2 == 0)
            res += tasks[i].andrei_score;
        else
            res -= tasks[i].bogdan_score;
    
    if (res > 0)
        cout << "Andrei";
    else if (res == 0)
        cout << "amandoi";
    else
        cout << "Bogdan";
}