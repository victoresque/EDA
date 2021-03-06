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
    // Optimize iteratively until opt_count < opt_thres    
    int opt_count = 0;
    int opt_thres = 0; //ov.size() / 200;
    int iter[5] = {0, 32, 1, 1, 1};
    do {
        opt_count = 0;
        for (int i=1; i<=4; i++)
            for (int j=0; j<iter[i]; j++)
                for (auto& it: r2o) {
                    const pair<int, int>& redund_via = it.first;
                    vector<pair<int, int>>& original_vias = it.second;
                    if (original_vias.size() == i) {
                        opt_count += 1;
                        pair<int, int> original_via = original_vias[0];
                        for (int k=0; k<original_vias.size(); k++) 
                            if (o2r[original_vias[k]].size() < o2r[original_via].size())
                                original_via = original_vias[k];
                        result[redund_via] = original_via;
                        for (auto& via: o2r[original_via])
                            if (via != redund_via) {
                                vector<pair<int, int>>& v = r2o[via];
                                v.erase(remove(v.begin(), v.end(), original_via), v.end());
                            }
                        original_vias.clear();
                    }
                }
    } while (opt_count > opt_thres);

    map<pair<int, int>, int> answer;
    cout << "# Objective value = " << result.size() << endl;
    fout << "# Objective value = " << result.size() << endl;
    
    for (auto& it: result) {
        const pair<int, int>& redund_via = it.first;
        pair<int, int>& original_via = it.second;
        int ox = original_via.first, oy = original_via.second;
        int rx = redund_via.first, ry = redund_via.second;
        for (int i=1; i<=4; i++) 
            if (rx == ox+dx[i] && ry == oy+dy[i])
                answer[original_via] = i;
    }

    for (auto& via: ov) 
        for (int i=1; i<=4; i++) 
            if (ovs.count(make_pair(via.first+dx[i], via.second+dy[i]))) continue;
            else fout << "R_" << via.first << "_" << via.second << "_" << i << " " << (answer[via] == i) << endl;
    
    return 0;
}
