#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

/*Fie S un șir de numere naturale { s1, s2, ... , sn } și K un număr na-
tural, cu K ≥ si pentru orice i între 1 și n.*/


/*Scrieți un algoritm pseudo-polinomial care găsește suma maximă,
dar care să fie ≤ K, ce poate fi formată din elementele din S (numere
întregi, pozitive, luate cel mult o singură dată). Indicați complexi-
tatea de timp/spațiu a algoritmului propus de voi și justificați de ce
acesta este corect (de ce soluția găsită este optimă).*/

// Complexitate: O(n * K), unde n este numarul de elemente din S
// Justificare: Folosim o matrice de n linii si K coloane unde 
// dynamic_matrix[i][j] = suma maxima <= j folosind doar primele i numere

// Demonstram optimalitatea prin inductie
// Pas 0: 0 numere => suma maxima e 0 (adev pt algoritmul nostru si pt optim) 
// Pas i: i numere => optim(i) = dynamic_matrix[i][K] (presupunere)
// Pas i + 1: avem 2 optiuni
//      - nu alegem S[i+1] => optim(i+1) = optim(i) = dynamic_matrix[i][K] 
//      - alegem S[i+1] => suma maxima ramasa este K - S[i+1], iar numerele ramase sunt
//        cele pana la pasul i. Dar aceasta suma maxima ramasa a fost calculata la pasul i
//        pe care l-am presupus corect (care este dynamic_matrix[i][K - S[i+1]])
//      - Optimul este maximul dintre cele 2 optiuni, exact ce face algoritmul nostru
//      => Pas i + 1 adevarat si deci algoritmul este optim

int a(ifstream& fin){
    int elem_count, capacity;
    fin >> elem_count >> capacity;

    vector<int> elements(elem_count);
    for (int i = 0; i < elem_count; ++i)
        fin >> elements[i];
    
    vector<vector<long int>> dynamic_matrix(elem_count, vector<long int>(capacity + 1));
    // Using the first object
    for (int i = elements[0]; i <= capacity; ++i)
        dynamic_matrix[0][i] = elements[0];
    
    // max(backpack without object i, 
    // backpack with object i (which means we take the maximum of the backpack with a decreased capacity))
    for (int i = 1; i < elem_count; ++i)
        for (int j = 1; j <= capacity; ++j)
            // Checking if object fits
            if (j - elements[i] >= 0)
                dynamic_matrix[i][j] = max(dynamic_matrix[i - 1][j],
                                           dynamic_matrix[i - 1][j - elements[i]] + elements[i]);
            else
                dynamic_matrix[i][j] = dynamic_matrix[i - 1][j];
    
    // for (const auto& l : dynamic_matrix){
    //     for (const auto& v : l)
    //         cout << v << " ";
    //     cout << '\n';
    // }

    return dynamic_matrix.back().back();
}

/*Scrieți un algoritm aproximativ care calculează o sumă cel puțin pe
jumătate de mare ca cea optimă, dar rulează în timp O(n) și com-
plexitate spațiu O(1).*/
int b(ifstream& fin){
    int elem_count, capacity;
    fin >> elem_count >> capacity;

    int sum = 0;
    int curr;

    for (int i = 0; i < elem_count; ++i){
        fin >> curr;

        if (sum + curr < capacity)
            sum += curr;
        else
            return max(sum, curr);
    }
    return 0;
}

int main(){
    ifstream fin("data.in");
    ofstream fout("data.out");

    if (!fin){
        cerr << "[ERROR] main: Could not open data.in\n";
        return -1;
    }

    fout << "Optimal Algo: " << a(fin) << '\n';
    fin.seekg(0);
    fout << "Approx Algo: " << b(fin) << '\n';
}