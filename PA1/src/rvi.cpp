#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <algorithm>
#include <cassert>
#include <cstdio>
using namespace std;

int main (int argc, char** argv) {
    assert(argc == 3);
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);

    int via_count;
    int dx[5] = {0, 0, 1, 0, -1};
    int dy[5] = {0, 1, 0, -1, 0};

    vector<pair<int, int>> ov; // vector of all original vias in input order
    set<pair<int, int>> ovs; // set of all original vias
    map<pair<int, int>, vector<pair<int, int>>> r2o; // redundant vias -> original vias
    map<pair<int, int>, vector<pair<int, int>>> o2r; // original vias -> redundant vias
    map<pair<int, int>, pair<int, int>> result; // chosen redundant vias (r -> o)

    fin >> via_count;
    // build map (redundant -> original)
    for (int i=0; i<via_count; i++) {
        int x, y;
        fin >> x >> y;
        ov.push_back(make_pair(x, y));
        ovs.insert(make_pair(x, y));
        r2o[make_pair(x+1, y)].push_back(make_pair(x, y));
        r2o[make_pair(x-1, y)].push_back(make_pair(x, y));
        r2o[make_pair(x, y+1)].push_back(make_pair(x, y));
        r2o[make_pair(x, y-1)].push_back(make_pair(x, y));
    }
    // clear coordinates that are already vias
    for (auto& it: ov) 
        r2o[it].clear();
    // build map (original -> redundant)
    for (auto& it: r2o) {
        const pair<int, int>& redund_via = it.first;
        vector<pair<int, int>>& original_vias = it.second;
        for (auto& via: original_vias) 
            o2r[via].push_back(redund_via);
    }
    fout << "Maximize\n";
    bool firstInAll = true;
    for (auto& it: o2r) {
        const pair<int, int>& original_via = it.first;
        vector<pair<int, int>>& redund_vias = it.second;
        if (!firstInAll) fout << " +\n";
        firstInAll = false;
        fout << "    ";
        bool firstInLine = true;
        for (auto& via: redund_vias) {
            if (!firstInLine) fout << " + ";
            firstInLine = false;
            fout << "R_" << original_via.first << "_" << original_via.second << "_";
            for (int i=1; i<=4; i++) {
                if (original_via.first + dx[i] == via.first
                    && original_via.second + dy[i] == via.second)
                    fout << i;
            }
        }
    }
    fout << "\nSubject To\n";
    for (auto& it: o2r) {
        const pair<int, int>& original_via = it.first;
        vector<pair<int, int>>& redund_vias = it.second;
        fout << "    R_" << original_via.first << "_" << original_via.second << ": ";
        bool firstInLine = true;
        for (auto& via: redund_vias) {
            if (!firstInLine) fout << " + ";
            firstInLine = false;
            fout << "R_" << original_via.first << "_" << original_via.second << "_";
            for (int i=1; i<=4; i++) {
                if (original_via.first + dx[i] == via.first
                    && original_via.second + dy[i] == via.second)
                    fout << i;
            }
        }
        fout << " <= 1\n";
    }
    int c_idx = 1;
    for (auto& it: r2o) {
        const pair<int, int>& redund_via = it.first;
        vector<pair<int, int>>& original_vias = it.second;
        if (original_vias.size() > 1) {
            fout << "    C" << c_idx++ << ": ";
            bool firstInLine = true;
            for (auto& via: original_vias) {
                for (int i=1; i<=4; i++) {
                    if (via.first + dx[i] == redund_via.first
                        && via.second + dy[i] == redund_via.second) {
                        if (!firstInLine) fout << " + ";
                        firstInLine = false;
                        fout << "R_" << via.first << "_" << via.second << "_" << i;
                    }
                }
            }
            fout << " <= 1\n";
        }
    }
    fout << "Bounds\n";
    fout << "Binary\n";
    for (auto& it: o2r) {
        const pair<int, int>& original_via = it.first;
        vector<pair<int, int>>& redund_vias = it.second;
        fout << "   ";
        for (auto& via: redund_vias) {
            fout << " R_" << original_via.first << "_" << original_via.second << "_";
            for (int i=1; i<=4; i++) {
                if (original_via.first + dx[i] == via.first
                    && original_via.second + dy[i] == via.second)
                    fout << i;
            }
        }
        fout << "\n";
    }
    fout << "End\n";
    return 0;
}
