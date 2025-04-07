#include <iostream>
#include <fstream>
#include <unordered_map>
#include <stdint.h>
using namespace std;

//100p
ifstream fin("data.in");
ofstream fout("date.out");


string convertNumber(uint64_t number){
    string res(4, '0');
    int i = 3;
    while (number){
        char c = number % 62;
        if (c >= 0 && c <= 9)
            res[i--] = c + 48;
        else if (c >= 10 && c <= 35)
            res[i--] = 'A' + c - 10;
        else if (c >= 36 && c <= 61)
            res[i--] = 'a' + c - 36;
        number = number / 62;
    }
    return res;
}

int main(){
    int nr_q;
    fin >> nr_q;

    string short_url_pre = "https://ShortURL.ro/";
    unordered_map<string, string> url_map;
    uint64_t last_number = 0;
    while (nr_q--){
        int nr;
        string url;
        fin >> nr >> url;
        // Long url
        if (nr == 1){
            string short_url = short_url_pre + convertNumber(last_number++);
            url_map[url] = short_url;
            url_map[short_url] = url;
            fout << short_url << '\n';
        }
        else{
            auto long_url = url_map.find(url);
            if (url_map.find(url) != url_map.end())
                fout << long_url->second << '\n';
            else
                fout << "nu exista\n";
        }
    }
}