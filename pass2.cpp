#include <bits/stdc++.h>
using namespace std;

string pad3(string const &s)
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
    ifstream sc("symtable.txt");
    if (!sc)
    {
        cout << "Unable to open symbol table file" << endl;
        return 1;
    }
    unordered_map<int, string> symaddr;
    {
        int no;
        string name, memo;
        while (sc >> no >> name >> memo)
        {
            symaddr[no] = memo;
        }
    }
    ifstream lt("littable.txt");
    if (!lt)
    {
        cout << "Unable to open literal table file" << endl;
        return 1;
    }
    unordered_map<int, string> litaddr;
    {
        int no;
        string name, memo;
        while (lt >> no >> name >> memo)
        {
            litaddr[no] = memo;
        }
    }
    ifstream icin("ic.txt");
    if (!icin)
    {
        cout << "Unable to open intermediate code file" << endl;
        return 1;
    }
    ofstream mc("machine_code.txt");
    if (!mc)
    {
        cout << "Unable to open machine code file" << endl;
        return 1;
    }
    string line;
    while (getline(icin, line))
    {
        string lc, ic;
        stringstream ss(line);
        ss >> lc >> ic;

        vector<string> groups;
        string temp;
        for (int i = 0; i < ic.size(); i++)
        {
            if (ic[i] == '(')
            {
                i++;
                temp.clear();
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
                string reg = "0";
                string memo = "000";
                if (opcode == "00")
                {
                    MC = opcode + "\t" + reg + "\t" + memo;
                }
                else
                {
                    int idx = 1;
                    if (groups.size() > 1 && groups[idx].find(",") == string::npos)
                    {
                        reg = groups[idx];
                        idx++;
                    }
                    if (groups.size() > idx)
                    {
                        string g2 = groups[idx];
                        if (g2.substr(0, 2) == "S,")
                        {
                            string idstr = g2.substr(2);
                            int id = idstr.empty() ? -1 : stoi(idstr);
                            memo = symaddr.count(id) ? pad3(symaddr[id]) : "000";
                        }
                        else if (g2.substr(0, 2) == "L,")
                        {
                            string idstr = g2.substr(2);
                            int id = idstr.empty() ? -1 : stoi(idstr);
                            memo = litaddr.count(id) ? pad3(litaddr[id]) : "000";
                        }
                        else if (g2.substr(0, 2) == "C,")
                        {
                            memo = g2.substr(2);
                        }
                    }
                    MC = opcode + "\t" + reg + "\t" + memo;
                }
            }
            else if (g0.substr(0, 3) == "DL,")
            {
                if (g0.substr(3) == "01")
                {
                    string val = "000";
                    if (groups.size() > 1 && groups[1].substr(0, 2) == "C,")
                    {
                        string val = groups[1].substr(2);
                        MC = "00\t0\t" + pad3(val);
                    }
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
    cout << "Pass 2 done" << endl;
    return 0;
}
