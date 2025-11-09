#include <bits/stdc++.h>
using namespace std;

struct OP
{
    string opcode, mclass, mnemonic;
};
struct Sym
{
    int no;
    string name, addr;
};
struct Lit
{
    int no;
    string name, addr;
};
struct Pool
{
    int no;
    string litno;
};

OP optab[] = {
    {"STOP", "IS", "00"}, {"ADD", "IS", "01"}, {"SUB", "IS", "02"}, {"MULT", "IS", "03"}, {"MOVER", "IS", "04"}, {"MOVEM", "IS", "05"}, {"COMP", "IS", "06"}, {"BC", "IS", "07"}, {"DIV", "IS", "08"}, {"READ", "IS", "09"}, {"PRINT", "IS", "10"}, {"START", "AD", "01"}, {"END", "AD", "02"}, {"ORIGIN", "AD", "03"}, {"EQU", "AD", "04"}, {"LTORG", "AD", "05"}, {"DC", "DL", "01"}, {"DS", "DL", "02"}};

Sym ST[50];
Lit LT[50];
Pool PT[50];

int scnt = 0, lcnt = 0, nlcnt = 0, pcnt = 0;
int poolStartIndex = 0;

unordered_map<string, int> reg = {
    {"AREG", 1}, {"BREG", 2}, {"CREG", 3}, {"DREG", 4}};
unordered_map<string, int> cond = {
    {"LT", 1}, {"LE", 2}, {"EQ", 3}, {"GT", 4}, {"GE", 5}, {"ANY", 6}};

int getOP(string s)
{
    for (int i = 0; i < 18; i++)
        if (optab[i].opcode == s)
            return i;
    return -1;
}

bool presentST(string s)
{
    for (int i = 0; i < scnt; i++)
        if (ST[i].name == s)
            return true;
    return false;
}

int getSymID(string s)
{
    for (int i = 0; i < scnt; i++)
        if (ST[i].name == s)
            return i;
    return -1;
}

bool presentLTCurrentPool(string s)
{
    for (int i = poolStartIndex; i < lcnt; i++)
        if (LT[i].name == s)
            return true;
    return false;
}

int getLitID(string s)
{
    for (int i = 0; i < lcnt; i++)
        if (LT[i].name == s)
            return i;
    return -1;
}

bool isNumber(const string &s)
{
    for (char c : s)
        if (!isdigit(c))
            return false;
    return !s.empty();
}

int main()
{
    ifstream fin("input.txt");
    ofstream ic("ic.txt"), st("symtable.txt"), lt("littable.txt");

    string label, opcode, op1, op2;
    int LC = 0;

    while (fin >> label >> opcode >> op1 >> op2)
    {
        int id = getOP(opcode);
        string IC = "", lc = "---";
        if (id == -1)
            continue;

        IC = "(" + optab[id].mclass + "," + optab[id].mnemonic + ")";

        // START
        if (opcode == "START")
        {
            if (op1 != "NAN")
            {
                LC = stoi(op1);
                IC += "(C," + op1 + ")";
            }
            ic << lc << "\t" << IC << endl;
            continue;
        }

        // Label handling
        if (label != "NAN")
        {
            if (!presentST(label))
                ST[scnt++] = {scnt, label, to_string(LC)};
            else
                ST[getSymID(label)].addr = to_string(LC);
        }

        // ORIGIN
        if (opcode == "ORIGIN")
        {
            string token1, token2;
            char op;
            size_t found = op1.find('+');
            op = (found != string::npos) ? '+' : '-';
            stringstream ss(op1);
            getline(ss, token1, op);
            getline(ss, token2);
            if (op == '+')
                LC = stoi(ST[getSymID(token1)].addr) + stoi(token2);
            else
                LC = stoi(ST[getSymID(token1)].addr) - stoi(token2);
            IC += "(S,0" + to_string(ST[getSymID(token1)].no) + ")" + op + token2;
            ic << lc << "\t" << IC << endl;
            continue;
        }

        // EQU
        if (opcode == "EQU")
        {
            string addr = ST[getSymID(op1)].addr;
            if (presentST(label))
                ST[getSymID(label)].addr = addr;
            else
                ST[scnt++] = {scnt, label, addr};
            IC += "(S,0" + to_string(ST[getSymID(op1)].no) + ")";
            ic << lc << "\t" << IC << endl;
            continue;
        }

        // LTORG / END
        if (opcode == "LTORG" || opcode == "END")
        {
            ic << lc << "\t" << IC << endl;
            for (int i = lcnt - nlcnt; i < lcnt; i++)
            {
                lc = to_string(LC);
                IC = "(DL,01)(C," + string(1, LT[i].name[2]) + ")";
                LT[i].addr = to_string(LC++);
                ic << lc << "\t" << IC << endl;
            }
            if (nlcnt > 0)
                PT[pcnt++] = {pcnt + 1, "#" + to_string(LT[lcnt - nlcnt].no)};
            nlcnt = 0;
            poolStartIndex = lcnt;
            if (opcode == "END")
                break;
            continue;
        }

        lc = to_string(LC);

        // DS
        if (opcode == "DS")
        {
            IC += "(C," + op1 + ")";
            LC += stoi(op1);
        }
        // DC
        else if (opcode == "DC")
        {
            IC += "(C," + op1 + ")";
            LC++;
        }
        // Imperative Statements
        else if (optab[id].mclass == "IS")
        {
            if (opcode != "STOP")
            {
                if (opcode == "BC")
                    IC += "(" + to_string(cond[op1]) + ")";
                else if (op1 != "NAN")
                    IC += "(" + to_string(reg[op1]) + ")";
            }
            if (op2 != "NAN")
            {
                if (op2[0] == '=')
                {
                    if (!presentLTCurrentPool(op2))
                    {
                        LT[lcnt++] = {lcnt, op2, "0"};
                        nlcnt++;
                    }
                    IC += "(L,0" + to_string(getLitID(op2)) + ")";
                }
                else if (isNumber(op2))
                {
                    IC += "(C," + op2 + ")";
                }
                else
                {
                    if (!presentST(op2))
                        ST[scnt++] = {scnt, op2, "0"};
                    IC += "(S,0" + to_string(getSymID(op2)) + ")";
                }
            }
            LC++;
        }

        ic << lc << "\t" << IC << endl;
    }

    for (int i = 0; i < scnt; i++)
        st << ST[i].no << "\t" << ST[i].name << "\t" << ST[i].addr << endl;

    for (int i = 0; i < lcnt; i++)
        lt << LT[i].no << "\t" << LT[i].name << "\t" << LT[i].addr << endl;

    return 0;
}
