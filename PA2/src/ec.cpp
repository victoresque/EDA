#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
using namespace std;

const string suffix = "_";
const string miter_prefix = "$$$MITER_";
const string miter_out = miter_prefix + "OUT$$$";

struct Gate {
    Gate (string type, string fanout, vector<string> fanin) {
        for (auto & c: type) c = toupper(c);
        this->type = type;
        this->fanout = fanout;
        this->fanin = fanin;
    }
    string type;
    string fanout;
    vector<string> fanin;
};
typedef vector<Gate> Netlist;

Netlist getNetlist (string filename, string suffix = "") {
    string tmp;
    Netlist netlist;
    unordered_set<string> inputs;
    ifstream fin(filename);

    while (getline(fin, tmp)) {
        tmp.erase(remove(tmp.begin(), tmp.end(), ' '), tmp.end());
        if (tmp.empty() || tmp[0] == '#') {
            continue;
        }
        else if (tmp.substr(0, 6) == "INPUT(") {
            tmp = tmp.substr(6);
            tmp.pop_back();
            inputs.insert(tmp);
            netlist.push_back(Gate("INPUT", tmp, {tmp}));
        }
        else if (tmp.substr(0, 7) == "OUTPUT(") {
            tmp = tmp.substr(7);
            tmp.pop_back();
            tmp = tmp + suffix;
            netlist.push_back(Gate("OUTPUT", tmp, {tmp}));
        }
        else {
            int eqpos = tmp.find('=');
            string no;
            no = tmp.substr(0, eqpos) + suffix;
            tmp = tmp.substr(eqpos + 1);
            int lbrackpos = tmp.find('(');
            string gate_type = tmp.substr(0, lbrackpos);
            vector<string> nis;
            tmp = tmp.substr(lbrackpos+1);
            tmp = tmp.substr(0, tmp.length()-1);
            while (int commapos = tmp.find(',')) {
                if (commapos == string::npos) break;
                string node = tmp.substr(0, commapos);
                nis.push_back(node + (inputs.count(node) ? "" : suffix));
                tmp = tmp.substr(commapos+1);
            }
            nis.push_back(tmp + (inputs.count(tmp) ? "" : suffix));
            netlist.push_back(Gate(gate_type, no, nis));
        }
    }

    return netlist;
}

Netlist getMiter (const Netlist & net1, const Netlist & net2) {
    Netlist miter = net1;
    miter.insert(miter.end(), net2.begin(), net2.end());

    for (const auto & gate: net1)
        if (gate.type == "OUTPUT")
            miter.push_back(Gate("XOR", ::miter_prefix + gate.fanout, {gate.fanout, gate.fanout + ::suffix}));

    Gate miter_out = Gate("OR", ::miter_out, {});
    for (const auto & gate: miter)
        if (gate.fanout.substr(0, ::miter_prefix.length()) == ::miter_prefix)
            miter_out.fanin.push_back(gate.fanout);
    miter.push_back(miter_out);

    return miter;
}

vector<string> getCNFs (const Gate & gate, unordered_map<string, int> & index) {
    vector<string> cnfs;
    string cnf;

    if (gate.type == "INPUT") {
    }
    else if (gate.type == "OUTPUT") {
    }
    else if (gate.type == "AND") {
        for (const auto & in: gate.fanin) cnf = cnf + "-" + to_string(index[in]) + " ";
        cnfs.push_back(cnf + to_string(index[gate.fanout]) + " 0");
        for (const auto & in: gate.fanin)
            cnfs.push_back(to_string(index[in]) + " " + "-" + to_string(index[gate.fanout]) + " 0");
    }
    else if (gate.type == "NAND") {
        for (const auto & in: gate.fanin) cnf = cnf + "-" + to_string(index[in]) + " ";
        cnfs.push_back(cnf + "-" + to_string(index[gate.fanout]) + " 0");
        for (const auto & in: gate.fanin)
            cnfs.push_back(to_string(index[in]) + " " + to_string(index[gate.fanout]) + " 0");
    }
    else if (gate.type == "OR") {
        for (const auto & in: gate.fanin) cnf = cnf + to_string(index[in]) + " ";
        cnfs.push_back(cnf + "-" + to_string(index[gate.fanout]) + " 0");
        for (const auto & in: gate.fanin)
            cnfs.push_back("-" + to_string(index[in]) + " " + to_string(index[gate.fanout]) + " 0");
    }
    else if (gate.type == "NOR") {
        for (const auto & in: gate.fanin) cnf = cnf + to_string(index[in]) + " ";
        cnfs.push_back(cnf + to_string(index[gate.fanout]) + " 0");
        for (const auto & in: gate.fanin)
            cnfs.push_back("-" + to_string(index[in]) + " " + "-" + to_string(index[gate.fanout]) + " 0");
    }
    else if (gate.type == "NOT") {
        string a = to_string(index[gate.fanin[0]]);
        string c = to_string(index[gate.fanout]);
        cnfs.push_back("-" + a + " " + "-" + c + " 0");
        cnfs.push_back(a + " " + c + " 0");
    }
    else if (gate.type == "XOR") {
        string a = to_string(index[gate.fanin[0]]);
        string b = to_string(index[gate.fanin[1]]);
        string c = to_string(index[gate.fanout]);
        cnfs.push_back("-" + a + " " + "-" + b + " " + "-" + c + " 0");
        cnfs.push_back(a + " " + b + " " + "-" + c + " 0");
        cnfs.push_back(a + " " + "-" + b + " " + c + " 0");
        cnfs.push_back("-" + a + " " + b + " " + c + " 0");
    }
    else if (gate.type == "EQV" || gate.type == "XNOR") {
        string a = to_string(index[gate.fanin[0]]);
        string b = to_string(index[gate.fanin[1]]);
        string c = to_string(index[gate.fanout]);
        cnfs.push_back(a + " " + b + " " + c + " 0");
        cnfs.push_back("-" + a + " " + "-" + b + " " + c + " 0");
        cnfs.push_back("-" + a + " " + b + " " + "-" + c + " 0");
        cnfs.push_back(a + " " + "-" + b + " " + "-" + c + " 0");
    }
    else if (gate.type == "BUF") {
        string a = to_string(index[gate.fanin[0]]);
        string c = to_string(index[gate.fanout]);
        cnfs.push_back("-" + a + " " + c + " 0");
        cnfs.push_back(a + " " + "-" + c + " 0");
    }
    else {
    }

    return cnfs;
}

void writeCNF (const Netlist & miter, string filename) {
    ofstream fout(filename);
    unordered_map<string, int> index;

    for (const auto & gate: miter)
        if (!index.count(gate.fanout))
            index[gate.fanout] = index.size() + 1;

    vector<string> clauses;
    for (const auto & gate: miter) {
        vector<string> cnf = getCNFs(gate, index);
        clauses.insert(clauses.end(), cnf.begin(), cnf.end());
    }
    clauses.push_back(to_string(index[miter_out]) + " 0");

    fout << "p cnf " << index.size() << " " << clauses.size() << endl;
    for (const auto & clause: clauses)
        fout << clause << endl;
}

int main (int argc, char ** argv) {
    Netlist netlist1 = getNetlist(argv[1]);
    Netlist netlist2 = getNetlist(argv[2], ::suffix);
    Netlist miter = getMiter(netlist1, netlist2);
    writeCNF(miter, argv[3]);
    return 0;
}
