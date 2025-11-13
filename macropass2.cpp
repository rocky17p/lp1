#include <bits/stdc++.h>
using namespace std;

string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return (a == string::npos) ? "" : s.substr(a, b - a + 1);
}

vector<string> splitArgs(const string &s) {
    vector<string> args; string arg; stringstream ss(s);
    while (getline(ss, arg, ',')) args.push_back(trim(arg));
    return args;
}

struct MNTEntry { string name; int mdtIndex; };
vector<MNTEntry> MNT;
vector<string> MDT;
map<string, vector<string>> PNTAB;
map<string, map<string, string>> KPDTAB;

void expandMacro(const string &name, const vector<string> &actuals, ofstream &out) {
    if (!PNTAB.count(name)) return;

    // Prepare argument list (ALA)
    vector<string> formals = PNTAB[name], ALA(formals.size());
    for (size_t i = 0; i < formals.size(); ++i)
        ALA[i] = KPDTAB[name].count(formals[i]) ? KPDTAB[name][formals[i]] : formals[i];
    for (size_t i = 0; i < actuals.size() && i < ALA.size(); ++i)
        if (!actuals[i].empty()) ALA[i] = actuals[i];

    // Find MDT entry
    int start = -1;
    for (auto &m : MNT) if (m.name == name) { start = m.mdtIndex - 1; break; }
    if (start == -1) return;

    // Expand lines until MEND
    for (int i = start + 1; i < MDT.size() && MDT[i] != "MEND"; ++i) {
        string line = MDT[i];
        for (size_t j = 0; j < ALA.size(); ++j)
            for (size_t pos; (pos = line.find("#" + to_string(j + 1))) != string::npos;)
                line.replace(pos, 2, ALA[j]);
        out << line << "\n";
    }
}

int main() {
    ifstream mnt("MNT.txt"), mdt("MDT.txt"), pnt("PNTAB.txt"), kpd("KPDTAB.txt"), ic("IC.txt");
    ofstream out("output.txt");
    string line;

    // Load MNT
    while (getline(mnt, line)) {
        string name; int i, p, k, m;
        if (stringstream(line) >> i >> name >> p >> k >> m) MNT.push_back({name, m});
    }

    // Load MDT
    while (getline(mdt, line)) {
        size_t t = line.find('\t');
        MDT.push_back(trim(t != string::npos ? line.substr(t + 1) : line));
    }

    // Load KPDTAB
    while (getline(kpd, line)) {
        int i; string mac, k, v;
        if (stringstream(line) >> i >> mac >> k >> v) KPDTAB[mac][k] = v;
    }

    // Load PNTAB
    string curr;
    while (getline(pnt, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.find(':') != string::npos) curr = trim(line.substr(0, line.find(':')));
        else if (!curr.empty()) {
            int i; string p;
            if (stringstream(line) >> i >> p) PNTAB[curr].push_back(p);
        }
    }

    // Process intermediate code
    while (getline(ic, line)) {
        string op; stringstream ss(line); ss >> op;
        bool isMacro = false;
        for (auto &m : MNT)
            if (m.name == op) {
                string params = trim(line.substr(op.size()));
                while (!params.empty() && (params[0] == ',' || isspace(params[0]))) params.erase(params.begin());
                expandMacro(op, splitArgs(params), out);
                isMacro = true; break;
            }
        if (!isMacro) out << line << "\n";
    }

    cout << "✅ Pass 2 complete (simple macros only). See output.txt\n";
}
