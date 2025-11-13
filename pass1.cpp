#include <bits/stdc++.h>
using namespace std;

struct OP
{
    string opcode;
    string mclass;
    string mnemonic;
};

struct sym
{
    int no;
    string name, addr;
};
struct lit
{
    int no;
    string name, addr;
};
struct pool
{
    int no;
    string literalno;
};

OP optab[18] = {{"STOP", "IS", "00"}, {"ADD", "IS", "01"}, {"SUB", "IS", "02"}, {"MULT", "IS", "03"}, {"MOVER", "IS", "04"}, {"MOVEM", "IS", "05"}, {"COMP", "IS", "06"}, {"BC", "IS", "07"}, {"DIV", "IS", "08"}, {"READ", "IS", "09"}, {"PRINT", "IS", "10"}, {"START", "AD", "01"}, {"END", "AD", "02"}, {"ORIGIN", "AD", "03"}, {"EQU", "AD", "04"}, {"LTORG", "AD", "05"}, {"DS", "DL", "02"}, {"DC", "DL", "01"}};

int scount = 0, lcount = 0, nlcount = 0, pcount = 0, poolstartindex = 0;
sym ST[50];
lit LT[50];
pool PT[50];
int getop(string s)
{
    for (int i = 0; i < 18; i++)
    {
        if (optab[i].opcode == s)
        {
            return i;
        }
    }
    return -1;
}
int getsymid(string s)
{
    for (int i = 0; i < scount; i++)
    {
        if (ST[i].name == s)
        {
            return i;
        }
    }
    return -1;
}
bool presentST(string s)
{
    for (int i = 0; i < scount; i++)
    {
        if (ST[i].name == s)
        {
            return true;
        }
    }
    return false;
}
bool presentcurrentpool(string s)
{
    for (int i = poolstartindex; i < lcount; i++)
    {
        if (LT[i].name == s)
            return true;
    }
    return false;
}

int getlitid(string s)
{
    for (int i = 0; i < lcount; i++)
    {
        if (LT[i].name == s)
            return i;
    }
    return -1;
}
bool isnumber(const string &s)
{
    for (char c : s)
    {
        if (!isdigit(c))
        {
            return false;
        }
    }
    return true;
}

unordered_map<string, int> REG = {{"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}};
unordered_map<string, int> Cond = {{"LT", 1}, {"LE", 2}, {"EQ", 3}, {"GT", 4}, {"GE", 5}, {"ANY", 6}};

int LC = 0;
int main()
{
    ifstream fin("input.txt");
    ofstream icin("ic.txt"), symin("symtable.txt"), litin("littable.txt"), ptin("pooltable.txt");
    string label, opcode, op1, op2;
    while (fin >> label >> opcode >> op1 >> op2)
    {
        string ic = "", lc = "---";
        int id = getop(opcode);

        if (id == -1)
        {
            continue;
        }
        ic = "(" + optab[id].mclass + "," + optab[id].mnemonic + ")";
        if (opcode == "START")
        {
            ic += "(C," + op1 + ")";
            icin << lc << "\t" << ic << endl;
            LC = stoi(op1);
            continue;
        }
        if (label != "NAN")
        {
            if (presentST(label))
            {
                int symid = getsymid(label);
                ST[symid].addr = LC;
            }
            else
            {
                ST[scount] = {scount, label, to_string(LC)};
                scount++;
            }
        }
        if (opcode == "ORIGIN")
        {
            string token1, token2;
            char op;

            int pos = op1.find('+');
            if (pos == -1)
            {
                op = '-';
            }
            else
            {
                op = '+';
            }
            stringstream ss(op1);
            getline(ss, token1, op);
            getline(ss, token2);
            int symid = getsymid(token1);
            int addr = stoi(ST[symid].addr);
            if (op == '+')
            {
                LC = addr + stoi(token2);
            }
            else
            {
                LC = addr + stoi(token2);
            }
            ic += "(S," + to_string(ST[symid].no) + ")" + op + token2;
            icin << lc << "\t" << ic << endl;
            continue;
        }
        if (opcode == "EQU")
        {
            int labelid = getsymid(label);
            int opid = getsymid(op1);
            ST[labelid].addr = ST[opid].addr;
            ic += "(S," + to_string(ST[opid].no) + ")";
            icin << lc << "\t" << ic << endl;
            continue;
        }
        if (opcode == "LTORG" || opcode == "END")
        {
            icin << lc << "\t" << ic << endl;
            for (int i = lcount - nlcount; i < lcount; i++)
            {
                LT[i].addr = to_string(LC);
                ic = "(DL,01)(C," + string(1, LT[i].name[2]) + ")";
                icin << to_string(LC) << "\t" << ic << endl;
                LC++;
            }
            if (nlcount > 0)
            {
                PT[pcount] = {pcount, "#" + to_string(LT[lcount - nlcount].no)};
                nlcount = 0;
                poolstartindex = lcount;
            }
            if (opcode == "END")
            {
                break;
            }
            continue;
        }
        lc = to_string(LC);
        if (opcode == "DS")
        {
            ic += "(C," + op1 + ")";
            LC += stoi(op1);
        }
        else if (opcode == "DC")
        {
            ic += "(C," + op1 + ")";
            LC++;
        }
        else if (optab[id].mclass == "IS")
        {
            if (opcode != "STOP")
            {
                if (opcode == "BC")
                {
                    ic += "(" + to_string(Cond[op1]) + ")";
                }
                else if (opcode == "PRINT" || opcode == "READ")
                {
                    if (!presentST(op1))
                    {
                        ST[scount] = {scount, op1, "0"};
                        scount++;
                    }
                    int symid = getsymid(op1);
                    ic += "(S," + to_string(ST[symid].no) + ")";
                }
                else if (op1 != "NAN")
                {
                    ic += "(" + to_string(REG[op1]) + ")";
                }
            }
            if (op2 != "NAN")
            {
                if (op2.find('=') != string::npos)
                {
                    if (!presentcurrentpool(op2))
                    {
                        LT[lcount] = {lcount, op2, "0"};
                        lcount++;
                        nlcount++;
                    }
                    int litid = getlitid(op2);
                    ic += "(L," + to_string(LT[litid].no) + ")";
                }
                else if (isnumber(op2))
                {
                    ic += "(C," + op2 + ")";
                }
                else
                {
                    if (!presentST(op2))
                    {
                        ST[scount] = {scount, op2, "0"};
                        scount++;
                    }
                    int symid = getsymid(op2);
                    ic += "(S," + to_string(ST[symid].no) + ")";
                }
            }
            LC++;
        }
        icin << lc << "\t" << ic << endl;
    }
    for (int i = 0; i < scount; i++)
    {
        symin << ST[i].no << "\t" << ST[i].name << "\t" << ST[i].addr << endl;
    }
    for (int i = 0; i < lcount; i++)
    {
        litin << LT[i].no << "\t" << LT[i].name << "\t" << LT[i].addr << endl;
    }
    for (int i = 0; i < pcount; i++)
    {
        ptin << PT[i].no << "\t" << PT[i].literalno << endl;
    }
    return 0;
}
