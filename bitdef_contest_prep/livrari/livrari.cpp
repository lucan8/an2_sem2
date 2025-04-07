#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>
using namespace std;

ifstream fin("data.in");
ofstream fout("data.out");

struct Packet{
    int t1, t2;
    bool ok = false;
    static int t1_cost, t2_cost;
    Packet(int t1 = 0, int t2 = 0) : t1(t1), t2(t2){}

    int64_t getCost(){
        if (!ok)
            return -1;
        return t1 * t1_cost + t2 * t2_cost;
    }

    Packet operator+(const Packet& other){
        Packet res = Packet(this->t1 + other.t1, this->t2 + other.t2);
        res.ok = true;

        return res;
    }

    Packet choose(const Packet& packet){
        if (packet.t1 > packet.t2)
            return *this;
        return packet;
    }
};

Packet solver(int nr_packets, int t1_cap, int t1_cost, int t2_cap, int t2_cost){
    vector<Packet> packets(nr_packets + 1);
    packets[0].ok = true;

    Packet::t1_cost = t1_cost;
    Packet::t2_cost = t2_cost;

    for (int i = min(t1_cap, t2_cap); i < packets.size(); ++i){
        int64_t c1, c2;

        if (i - t1_cap < 0)
            c1 = -1;
        else
            c1 = packets[i - t1_cap].getCost();

        if (i - t2_cap < 0)
            c2 = -1;
        else
            c2 = packets[i - t2_cap].getCost();

        if (c1 < 0){
            if (c2 < 0) 
                continue;
            else
                packets[i] = packets[i - t2_cap] + Packet(0, 1);
        }
        else if (c2 < 0)
            packets[i] = packets[i - t1_cap] + Packet(1, 0);
        else if (c1 + t1_cost == c2 + t2_cost)
            packets[i] = packets[i - t1_cap].choose(packets[i - t2_cap]) + Packet(1, 0);
        else if (c1 + t1_cost < c2 + t2_cost)
            packets[i] = packets[i - t1_cap] + Packet(1, 0);
        else
            packets[i] = packets[i - t2_cap] + Packet(0, 1);
    }

    return packets.back();
}

int Packet::t1_cost = 0;
int Packet::t2_cost = 0;
int main(){
    int nr_q;
    fin >> nr_q;

    vector<Packet> res(nr_q);
    for (int i = 0; i < nr_q; ++i){
        int nr_packets, t1_cap, t2_cap, t1_cost, t2_cost;

        fin >> nr_packets >> t1_cap >> t1_cost >> t2_cap >> t2_cost;

        res[i] = solver(nr_packets, t1_cap, t1_cost, t2_cap, t2_cost);
    }

    for (auto& r : res){
        int64_t c_res = r.getCost();
        if (c_res < 0)
            fout << -1 << '\n';
        else
            fout << r.t1 << " " << r.t2 << "\n";
    }
} 