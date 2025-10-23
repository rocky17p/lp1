#include <bits/stdc++.h>
using namespace std;

static string pad3(const string &s){
    string t = s;
    t.erase(remove_if(t.begin(), t.end(), ::isspace), t.end());
    if(t.empty()) return "000";
    bool num = all_of(t.begin(), t.end(), ::isdigit);
    if(num){
        if(t.size() >= 3) return t.substr(t.size() - 3);
        return string(3 - t.size(), '0') + t;
    }
    return t;
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    unordered_map<int, string> symAddr;
    {
        ifstream st("symtable.txt");
        if(!st){
            cerr << "Failed to open symtable.txt\n";
            return 1;
        }
        int no; string name, addr;
        while(st >> no >> name >> addr){
            symAddr[no] = addr;
        }
    }

    unordered_map<int, string> litAddr;
    {
        ifstream lt("littable.txt");
        if(!lt){
            cerr << "Failed to open littable.txt\n";
            return 1;
        }
        int no; string lit, addr;
        while(lt >> no >> lit >> addr){
            litAddr[no] = addr;
        }
    }

    ifstream icin("ic.txt");
    if(!icin){
        cerr << "Failed to open ic.txt\n";
        return 1;
    }

    ofstream mc("machine_code.txt");
    if(!mc){
        cerr << "Failed to open machine_code.txt for write\n";
        return 1;
    }

    cout << "\n -- ASSEMBLER PASS-2 OUTPUT --\n";
    cout << "\n LC\t <INTERMEDIATE CODE>\t\t\tLC\t <MACHINE CODE>\n";

    string line;
    while(getline(icin, line)){
        if(line.empty()) continue;
        string lc, IC;
        size_t pos = line.find('\t');
        if(pos == string::npos){
            stringstream ss(line);
            ss >> lc >> IC;
        } else {
            lc = line.substr(0, pos);
            size_t pos2 = line.find_first_not_of("\t ", pos);
            if(pos2 == string::npos) IC = "";
            else IC = line.substr(pos2);
        }

        vector<string> groups;
        static const regex re(R"(\(([^)]+)\))");
        auto begin = sregex_iterator(IC.begin(), IC.end(), re);
        auto end = sregex_iterator();
        for(auto it = begin; it != end; ++it){
            groups.push_back((*it)[1].str());
        }

        string MC = "-NAN-";
        auto starts_with = [](const string &s, const string &p){ return s.rfind(p, 0) == 0; };

        if(!groups.empty()){
            string g0 = groups[0];
            if(starts_with(g0, "IS,")){
                string opcode = g0.substr(3);
                string reg = "0", mem = "000";
                if(opcode == "00"){
                    MC = opcode + "\t0\t000";
                } else {
                    int idx = 1;
                    if((int)groups.size() > 1 && groups[1].find(',') == string::npos){
                        reg = groups[1];
                        idx = 2;
                    }
                    if((int)groups.size() > idx){
                        string opg = groups[idx];
                        if(starts_with(opg, "S,")){
                            size_t comma = opg.find(',');
                            string after = (comma == string::npos) ? "" : opg.substr(comma + 1);
                            string idstr;
                            for(char c : after) if(isdigit(c)) idstr.push_back(c);
                            int sid = idstr.empty() ? -1 : stoi(idstr);
                            auto it = symAddr.find(sid);
                            mem = (it == symAddr.end()) ? "000" : pad3(it->second);
                        } else if(starts_with(opg, "L,")){
                            size_t comma = opg.find(',');
                            string after = (comma == string::npos) ? "" : opg.substr(comma + 1);
                            string idstr;
                            for(char c : after) if(isdigit(c)) idstr.push_back(c);
                            int lid = idstr.empty() ? -1 : stoi(idstr);
                            auto it = litAddr.find(lid);
                            mem = (it == litAddr.end()) ? "000" : pad3(it->second);
                        } else if(starts_with(opg, "C,")){
                            string val = opg.substr(2);
                            mem = pad3(val);
                        }
                    }
                    MC = opcode + "\t" + reg + "\t" + mem;
                }
            } else if(starts_with(g0, "DL,")){
                string dl = g0.substr(3);
                if(dl == "01"){
                    string val = "0";
                    if(groups.size() >= 2 && starts_with(groups[1], "C,")){
                        val = groups[1].substr(2);
                    }
                    MC = "00\t0\t" + pad3(val);
                } else if(dl == "02"){
                    MC = "-NAN-";
                }
            } else if(starts_with(g0, "AD,")){
                MC = "-NAN-";
            }
        }

        bool isAD03 = (!groups.empty() && groups[0] == "AD,03");
        if(isAD03){
            cout << " " << lc << "\t" << "(" << groups[0] << ")";
            for(size_t i = 1; i < groups.size(); ++i) cout << "(" << groups[i] << ")";
            cout << "\t\t\t" << lc << "\t" << MC << "\n";
        } else {
            cout << " " << lc << "\t";
            if(!groups.empty()){
                cout << "(" << groups[0] << ")";
                for(size_t i = 1; i < groups.size(); ++i) cout << "(" << groups[i] << ")";
            } else {
                cout << IC;
            }
            cout << "\t\t\t" << lc << "\t" << MC << "\n";
        }
        mc << lc << "\t" << MC << "\n";
    }
    return 0;
}
