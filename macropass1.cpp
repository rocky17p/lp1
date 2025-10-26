#include <bits/stdc++.h>
using namespace std;

struct MNTEntry
{
    int index;
    string name;
    int pntabPtr;
    int kpdtPtr;
    int mdtPtr;
};

int main()
{
    ifstream fin("input2.txt");
    ofstream mntFile("MNT.txt"), mdtFile("MDT.txt"), pntabFile("PNTAB.txt"), kpdtabFile("KPDTAB.txt"), ic("IC.txt");

    vector<string> MDT;
    vector<MNTEntry> MNT;
    map<string, vector<string>> PNTABs;
    vector<vector<string>> KPDTAB;

    int macroDepth = 0, macroIndex = 0, kpIndex = 1;
    bool headerNext = false;
    string currentMacro;
    vector<string> params;
    map<string, int> paramIndex;

    string line;
    while (getline(fin, line))
    {
        string L = line;

        // Detect MACRO
        if (L.find("MACRO") != string::npos)
        {
            macroDepth++;
            if (macroDepth == 1)
                headerNext = true;
            else
                MDT.push_back(L);
            continue;
        }

        // Process macro header
        if (headerNext)
        {
            stringstream ss(L);
            ss >> currentMacro;

            string rest;
            getline(ss, rest);
            rest.erase(remove(rest.begin(), rest.end(), ' '), rest.end());

            params.clear();
            paramIndex.clear();

            int kpdtStart = kpIndex;
            stringstream ps(rest);
            string p;
            int idx = 1;

            while (getline(ps, p, ','))
            {
                int eq = p.find('=');
                if (eq != -1)
                {
                    KPDTAB.push_back({currentMacro, p.substr(0, eq), p.substr(eq + 1)});
                    kpIndex++;
                }
                string paramName = p.substr(0, (eq != -1 ? eq : p.size()));
                params.push_back(paramName);
                paramIndex[paramName] = idx++;
            }

            macroIndex++;
            MNT.push_back({macroIndex, currentMacro, 1, (kpIndex > kpdtStart ? kpdtStart : 0), (int)MDT.size() + 1});
            MDT.push_back(rest);
            PNTABs[currentMacro] = params;

            headerNext = false;
            continue;
        }

        // Inside macro body
        if (macroDepth > 0)
        {
            if (L.find("MEND") != string::npos)
            {
                MDT.push_back("MEND");
                macroDepth--;
                continue;
            }

            for (auto &p : params)
            {
                string rep = "#" + to_string(paramIndex[p]);
                int pos = L.find(p);
                while (pos != -1)
                {
                    L.replace(pos, p.size(), rep);
                    pos = L.find(p);
                }
            }
            MDT.push_back(L);
        }
        else
        {
            ic << L << "\n";
        }
    }

    // Write MNT
    mntFile << "Index\tName\tPNTAB\tKPDTAB\tMDT\n";
    for (auto &e : MNT)
    {
        mntFile << e.index << "\t" << e.name << "\t" << e.pntabPtr << "\t"
                << e.kpdtPtr << "\t" << e.mdtPtr << "\n";
    }

    // Write MDT
    for (int i = 0; i < MDT.size(); i++)
        mdtFile << i + 1 << "\t" << MDT[i] << "\n";

    // Write PNTAB
    for (auto &kv : PNTABs)
    {
        pntabFile << kv.first << ":\n";
        for (int i = 0; i < kv.second.size(); i++)
            pntabFile << i + 1 << "\t" << kv.second[i] << "\n";
    }

    // Write KPDTAB
    for (int i = 0; i < KPDTAB.size(); i++)
    {
        kpdtabFile << i + 1 << "\t"
                   << KPDTAB[i][0] << "\t"  // Macro name
                   << KPDTAB[i][1] << "\t"  // Parameter name
                   << KPDTAB[i][2] << "\n"; // Default value
    }

    cout << "Pass 1 Done!\n";
    return 0;
}
