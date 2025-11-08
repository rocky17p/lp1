#include <bits/stdc++.h>
using namespace std;

string pad3(const string &s)
{
    string t = s;
    t.erase(remove(t.begin(), t.end(), ' '), t.end());
    if (t.empty())
        return "000";
    bool num = true;
    for (char c : t)
    {
        if (!isdigit(c))
        {
            num = false;
            break;
        }
    }
    if (num)
    {
        if (t.size() >= 3)
        {
            return t.substr(t.size() - 3);
        }
        return string(3 - t.size(), '0') + t;
    }
    return t;
}

int main()
{
    unordered_map<int, string> symaddr;
    {

        ifstream st("symtable.txt");
        if (!st)
        {
            cout << "unable to open symtable.txt" << endl;
            return 1;
        }
        int no;
        string name, addr;
        while (st >> no >> name >> addr)
        {
            symaddr[no] = addr;
        }
    }
    unordered_map<int, string> litaddr;
    {

        ifstream lt("littable.txt");
        if (!lt)
        {
            cout << "unable to open littable.txt" << endl;
            return 1;
        }
        int no;
        string name, addr;
        while (lt >> no >> name >> addr)
        {
            litaddr[no] = addr;
        }
    }
    ofstream mc("machine_code.txt");
    if (!mc)
    {
        cout << "failed to open machine_code" << endl;
        return 1;
    }
    ifstream icin("ic.txt");
    if (!icin)
    {
        cout << "failed to open ic.txt" << endl;
        return 1;
    }
    string line;
    while (getline(icin, line))
    {
        if (line.empty())
            continue;
        string lc, ic;
        stringstream ss(line);
        ss >> lc >> ic;
        vector<string> groups;
        string temp;
        for (int i = 0; i < ic.size(); i++)
        {
            if (ic[i] == '(')
            {
                temp.clear();
                i++;
                while (i < ic.size() && ic[i] != ')')
                {
                    temp += ic[i];
                    i++;
                }
                groups.push_back(temp);
            }
        }
        string MC = "-NAN-";
        if (!groups.empty())
        {
            string g0 = groups[0];

            if (g0.substr(0, 3) == "IS,")
            {
                string opcode = g0.substr(3);
                string reg = "0", mem = "000";
                if (opcode == "00")
                {
                    MC = opcode + "\t0\t000";
                }
                else
                {
                    int idx = 1;
                    if (groups.size() > 1 && groups[1].find(",") == string::npos)
                    {
                        reg = groups[1];
                        idx = 2;
                    }

                    if (groups.size() > idx)
                    {
                        string opg = groups[idx];
                        if (opg.substr(0, 2) == "S,")
                        {
                            string idstr = opg.substr(2);
                            int id = (idstr.empty()) ? -1 : stoi(idstr);
                            mem = symaddr.count(id) ? pad3(symaddr[id]) : "000";
                        }
                        else if (opg.substr(0, 2) == "L,")
                        {
                            string idstr = opg.substr(2);
                            int id = (idstr.empty()) ? -1 : stoi(idstr);
                            mem = litaddr.count(id) ? pad3(litaddr[id]) : "000";
                        }
                        else if (opg.substr(0, 2) == "C,")
                        {
                            string val = opg.substr(2);
                            mem = val;
                        }
                    }
                }
                MC = opcode + "\t" + reg + "\t" + mem;
            }
            else if (g0.substr(0, 3) == "DL,")
            {
                string dl = g0.substr(3);
                if (dl == "01")
                {
                    string val = "0";
                    if (groups.size() >= 2 && groups[1].substr(0, 2) == "C,")
                    {
                        val = groups[1].substr(2);
                    }
                    MC = "00\t0\t" + pad3(val);
                }
                else
                {
                    MC = "-NAN-";
                }
            }
            else if (g0.substr(0, 3) == "AD,")
            {
                MC = "-NAN-";
            }
        }
        mc << lc << "\t" << MC << endl;
    }
    return 0;
}
