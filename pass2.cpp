#include <bits/stdc++.h>

using namespace std;



static string pad3(const string &s) {

    // left-pad with zeros to width 3

    string t = s;

    // strip any spaces

    t.erase(remove_if(t.begin(), t.end(), ::isspace), t.end());

    if (t.empty()) return "000";

    // if it's numeric, pad

    bool num = all_of(t.begin(), t.end(), ::isdigit);

    if (num) {

        if (t.size() >= 3) return t.substr(t.size() - 3);

        return string(3 - t.size(), '0') + t;

    }

    // fallback

    return t;

}



int main() {

    ios::sync_with_stdio(false);

    cin.tie(nullptr);



    // Load Symbol Table

    unordered_map<int, string> symAddr;

    {

        ifstream st("symtable.txt");

        if (!st) { cerr << "Failed to open symtable.txt\n"; return 1; }

        int no; string name, addr;

        while (st >> no >> name >> addr) {

            symAddr[no] = addr;

        }

    }



    // Load Literal Table

    unordered_map<int, string> litAddr;

    {

        ifstream lt("littable.txt");

        if (!lt) { cerr << "Failed to open littable.txt\n"; return 1; }

        int no; string lit, addr;

        while (lt >> no >> lit >> addr) {

            litAddr[no] = addr;

        }

    }



    ifstream icin("ic.txt");

    if (!icin) { cerr << "Failed to open ic.txt\n"; return 1; }



    ofstream mc("machine_code.txt");

    if (!mc) { cerr << "Failed to open machine_code.txt for write\n"; return 1; }



    cout << "\n -- ASSEMBLER PASS-2 OUTPUT --\n";

    cout << "\n LC\t <INTERMEDIATE CODE>\t\t\tLC\t <MACHINE CODE>\n";



    string line;

    while (getline(icin, line)) {

        if (line.empty()) continue;



        // split into LC and IC (tab-separated; IC itself has no spaces from your pass-1)

        string lc, IC;

        {

            // find first tab

            size_t pos = line.find('\t');

            if (pos == string::npos) {

                // fallback: try space split

                stringstream ss(line);

                ss >> lc >> IC;

            } else {

                lc = line.substr(0, pos);

                // skip consecutive tabs/spaces

                size_t pos2 = line.find_first_not_of("\t ", pos);

                if (pos2 == string::npos) IC = "";

                else IC = line.substr(pos2);

            }

        }



        // Extract all (...) groups

        vector<string> groups;

        {

            static const regex re(R"(\(([^)]+)\))");

            auto begin = sregex_iterator(IC.begin(), IC.end(), re);

            auto end   = sregex_iterator();

            for (auto it = begin; it != end; ++it) {

                groups.push_back((*it)[1].str()); // content inside the parentheses

            }

        }



        // Default: no machine code

        string MC = "-NAN-";



        auto starts_with = [](const string &s, const string &p){ return s.rfind(p, 0) == 0; };



        if (!groups.empty()) {

            string g0 = groups[0]; // first group determines class

            // g0 like "IS,04" or "DL,01" or "AD,03"

            if (starts_with(g0, "IS,")) {

                string opcode = g0.substr(3); // two digits typically

                string reg = "0";

                string mem = "000";



                if (opcode == "00") {

                    // STOP

                    MC = opcode + "\t0\t000";

                } else {

                    // optional second group: register or condition code (for BC)

                    int idx = 1;

                    if ((int)groups.size() > 1 && groups[1].find(',') == string::npos) {

                        // plain number like "1".."6"

                        reg = groups[1];

                        idx = 2;

                    }

                    // operand group if present

                    if ((int)groups.size() > idx) {

                        string opg = groups[idx]; // could be S,0i  L,0j  C,val

                        if (starts_with(opg, "S,")) {

                            // format "S,0i" or "S,i"

                            size_t comma = opg.find(',');

                            string after = (comma == string::npos) ? "" : opg.substr(comma + 1);

                            // strip leading zeros/non-digits

                            string idstr;

                            for (char c : after) if (isdigit(c)) idstr.push_back(c);

                            int sid = idstr.empty() ? -1 : stoi(idstr);

                            auto it = symAddr.find(sid);

                            mem = (it == symAddr.end()) ? "000" : pad3(it->second);

                        } else if (starts_with(opg, "L,")) {

                            size_t comma = opg.find(',');

                            string after = (comma == string::npos) ? "" : opg.substr(comma + 1);

                            string idstr;

                            for (char c : after) if (isdigit(c)) idstr.push_back(c);

                            int lid = idstr.empty() ? -1 : stoi(idstr);

                            auto it = litAddr.find(lid);

                            mem = (it == litAddr.end()) ? "000" : pad3(it->second);

                        } else if (starts_with(opg, "C,")) {

                            string val = opg.substr(2);

                            mem = pad3(val);

                        }

                    }

                    MC = opcode + "\t" + reg + "\t" + mem;

                }

            }

            else if (starts_with(g0, "DL,")) {

                // DC / DS

                string dl = g0.substr(3); // "01" or "02"

                if (dl == "01") {

                    // DC: expect next group C,val

                    string val = "0";

                    if (groups.size() >= 2 && starts_with(groups[1], "C,")) {

                        val = groups[1].substr(2);

                    }

                    MC = "00\t0\t" + pad3(val);

                } else if (dl == "02") {

                    // DS: reserve, no machine code

                    MC = "-NAN-";

                }

            }

            else if (starts_with(g0, "AD,")) {

                // All assembler directives (START/END/ORIGIN/EQU/LTORG) -> no machine code

                MC = "-NAN-";

            }

        }



        // Pretty console output similar to your original pass-2 style

        // Special-case formatting for AD,03 (like you had)

        bool isAD03 = (!groups.empty() && groups[0] == "AD,03");

        if (isAD03) {

            cout << " " << lc << "\t" << "(" << groups[0] << ")";

            for (size_t i = 1; i < groups.size(); ++i) cout << "(" << groups[i] << ")";

            cout << "\t\t\t" << lc << "\t" << MC << "\n";

        } else {

            cout << " " << lc << "\t";

            if (!groups.empty()) {

                cout << "(" << groups[0] << ")";

                for (size_t i = 1; i < groups.size(); ++i) cout << "(" << groups[i] << ")";

            } else {

                cout << IC;

            }

            cout << "\t\t\t" << lc << "\t" << MC << "\n";

        }



        mc << lc << "\t" << MC << "\n";

    }



    return 0;

}
