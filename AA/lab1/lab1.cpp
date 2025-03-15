#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

struct BackpackObj{
    int weight, value;
};

int main(){
    ifstream fin("rucsac.in");
    ofstream fout("rucsac.out");

    int obj_count, backpack_capacity;
    fin >> obj_count >> backpack_capacity;

    vector<BackpackObj> objects(obj_count);
    for (int i = 0; i < obj_count; ++i)
        fin >> objects[i].weight >> objects[i].value;
    
    vector<vector<int>> dynamic_matrix(obj_count, vector<int>(backpack_capacity + 1));
    // Using the first object
    for (int i = objects[0].weight; i <= backpack_capacity; ++i)
        dynamic_matrix[0][i] = objects[0].value;
    
    // max(backpack without object i, 
    // backpack with object i (which means we take the maximum of the backpack with a decreased capacity))
    for (int i = 1; i < obj_count; ++i)
        for (int j = 1; j <= backpack_capacity; ++j)
        // Checking if object fits
            if (j - objects[i - 1].weight > 0)
                dynamic_matrix[i][j] = max(dynamic_matrix[i - 1][j],
                                           dynamic_matrix[i - 1][j - objects[i - 1].weight] + objects[i - 1].value);
            else
                dynamic_matrix[i][j] = dynamic_matrix[i - 1][j];
    
    fout << dynamic_matrix.back().back();
}