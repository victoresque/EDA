#include <fstream>
#include <cassert>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <cassert>
using namespace std;

int main (int argc, char** argv) {
    assert(argc == 4);
    ofstream fout(argv[3]);
    int via_count = stoi(argv[1]);
    int via_count_sqrt = int(sqrt(via_count)) + 1;
    float dim_ratio = stof(argv[2]);
    assert(dim_ratio >= 1);

    vector<pair<int, int>> vias;
    for (int i=1; i<=via_count_sqrt * dim_ratio; i++) 
        for (int j=1; j<=via_count_sqrt * dim_ratio; j++)
            vias.push_back(make_pair(i, j));

    random_device rd;
    mt19937 g(rd());
    shuffle(vias.begin(), vias.end(), g);

    fout << via_count << endl;
    for (int i=0; i<via_count; i++)
        fout << vias[i].first << " " << vias[i].second << endl;

    return 0;
}