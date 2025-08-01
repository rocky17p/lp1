#include <bits/stdc++.h>
using namespace std;

struct OPtab {
    string opcode;
    string mclass;
    string mnemonic;
};

struct symTable {
    int no;
    string sname;
    string addr;
};

struct litTable {
    int no;
    string lname;
    string addr;
};

struct poolTable {
    int no;
    string lno;
};

OPtab optab[18] = {
    {"STOP", "IS", "00"}, {"ADD", "IS", "01"}, {"SUB", "IS", "02"}, {"MULT", "IS", "03"},
    {"MOVER", "IS", "04"}, {"MOVEM", "IS", "05"}, {"COMP", "IS", "06"}, {"BC", "IS", "07"},
    {"DIV", "IS", "08"}, {"READ", "IS", "09"}, {"PRINT", "IS", "10"}, {"START", "AD", "01"},
    {"END", "AD", "02"}, {"ORIGIN", "AD", "03"}, {"EQU", "AD", "04"}, {"LTORG", "AD", "05"},
    {"DC", "DL", "01"}, {"DS", "DL", "02"}
};

symTable ST[50];
litTable LT[50];
poolTable PT[50];

int scnt = 0, lcnt = 0, nlcnt = 0, pcnt = 0;

int getOP(string s) {
    for (int i = 0; i < 18; ++i)
        if (optab[i].opcode == s) return i;
    return -1;
}

int getRegID(string s) {
    if (s == "AREG") return 1;
    if (s == "BREG") return 2;
    if (s == "CREG") return 3;
    if (s == "DREG") return 4;
    return -1;
}

int getConditionCode(string s) {
    if (s == "LT") return 1;
    if (s == "LE") return 2;
    if (s == "EQ") return 3;
    if (s == "GT") return 4;
    if (s == "GE") return 5;
    if (s == "ANY") return 6;
    return -1;
}

bool presentST(string s) {
    for (int i = 0; i < scnt; ++i)
        if (ST[i].sname == s) return true;
    return false;
}

int getSymID(string s) {
    for (int i = 0; i < scnt; ++i)
        if (ST[i].sname == s) return i;
    return -1;
}

bool presentLT(string s) {
    for (int i = 0; i < lcnt; ++i)
        if (LT[i].lname == s) return true;
    return false;
}

int getLitID(string s) {
    for (int i = 0; i < lcnt; ++i)
        if (LT[i].lname == s) return i;
    return -1;
}

int main() {
    ifstream fin("input3.txt");
    ofstream ic("ic.txt"), st("symtable.txt"), lt("littable.txt");

    string label, opcode, op1, op2;
    int LC = 0;

    cout << "\n -- ASSEMBLER PASS-1 OUTPUT --\n";
    cout << "\n <LABEL\t\tOPCODE\t\tOP1\t\tOP2\tLC\t\tINTERMEDIATE CODE>\n";

    while (fin >> label >> opcode >> op1 >> op2) {
        int id = getOP(opcode);
        string IC = "", lc = "---";

        if (id == -1) continue;

        IC = "(" + optab[id].mclass + "," + optab[id].mnemonic + ")";

        // Handle START
        if (opcode == "START") {
            if (op1 != "NAN") {
                LC = stoi(op1);
                IC += "(C," + op1 + ")";
            }
            cout << "  " << label << "\t\t" << opcode << "\t\t" << op1 << "\t\t" << op2
                 << "\t" << lc << "\t\t" << IC << endl;
            ic << lc << "\t" << IC << endl;
            continue;
        }

        // Update symbol table for labels (but don't add to IC)
        if (label != "NAN") {
            if (!presentST(label)) {
                ST[scnt++] = {scnt, label, to_string(LC)};
            } else {
                ST[getSymID(label)].addr = to_string(LC);
            }
        }

        // Handle ORIGIN
        if (opcode == "ORIGIN") {
            string token1, token2;
            char op;
            size_t found = op1.find('+');
            op = found != string::npos ? '+' : '-';
            stringstream ss(op1);
            getline(ss, token1, op);
            getline(ss, token2);
            if (op == '+')
                LC = stoi(ST[getSymID(token1)].addr) + stoi(token2);
            else
                LC = stoi(ST[getSymID(token1)].addr) - stoi(token2);
            IC += "(S,0" + to_string(ST[getSymID(token1)].no) + ")" + op + token2;
            cout << "  " << label << "\t\t" << opcode << "\t\t" << op1 << "\t\t" << op2
                 << "\t" << lc << "\t\t" << IC << endl;
            ic << lc << "\t" << IC << endl;
            continue;
        }

        // Handle EQU
        // if (opcode == "EQU") {
        //     if (presentST(label)) {
        //         ST[getSymID(label)].addr = ST[getSymID(op1)].addr;
        //     } else {
        //         ST[scnt++] = {scnt, label, ST[getSymID(op1)].addr};
        //     }
        //     continue;
        // }
        // Handle EQU
        if (opcode == "EQU") {
            string addr = ST[getSymID(op1)].addr;
            if (presentST(label)) {
                ST[getSymID(label)].addr = addr;
            } else {
                ST[scnt++] = {scnt, label, addr};
            }

            IC += "(S,0" + to_string(ST[getSymID(op1)].no) + ")";
            cout << "  " << label << "\t\t" << opcode << "\t\t" << op1 << "\t\t" << op2
                << "\t" << lc << "\t\t" << IC << endl;
            ic << lc << "\t" << IC << endl;
            continue;
        }


        // Handle LTORG and END
        if (opcode == "LTORG" || opcode == "END") {
            cout << "  " << label << "\t\t" << opcode << "\t\t" << op1 << "\t\t" << op2
                 << "\t" << lc << "\t\t" << IC << endl;
            ic << lc << "\t" << IC << endl;
            for (int i = lcnt - nlcnt; i < lcnt; ++i) {
                lc = to_string(LC);
                IC = "(DL,01)(C," + string(1, LT[i].lname[2]) + ")";
                LT[i].addr = to_string(LC++);
                cout << "\t\t\t\t\t\t\t" << lc << "\t\t" << IC << endl;
                ic << lc << "\t" << IC << endl;
            }
            if (nlcnt > 0)
                PT[pcnt++] = {pcnt + 1, "#" + to_string(LT[lcnt - nlcnt].no)};
            nlcnt = 0;
            if (opcode == "END") break;
            continue;
        }

        lc = to_string(LC);

        // Generate IC for instructions
        if (opcode == "DS") {
            IC += "(C," + op1 + ")";
            LC += stoi(op1);
        }
         else if (opcode == "DC") {
            IC += "(C," + op1 + ")";
            LC++;
        } 
        else if (optab[id].mclass == "IS") {
            if (op2 == "NAN") {
                if (op1 != "NAN") {
                    if (!presentST(op1))
                        ST[scnt++] = {scnt, op1, "0"};
                    IC += "(S,0" + to_string(ST[getSymID(op1)].no) + ")";
                }
            } else {
                IC += (opcode == "BC") ? "(" + to_string(getConditionCode(op1)) + ")"
                                        : "(" + to_string(getRegID(op1)) + ")";
                if (op2[0] == '=') {
                    if (!presentLT(op2)) {
                        LT[lcnt++] = {lcnt, op2, "0"};
                        nlcnt++;
                    }
                    IC += "(L,0" + to_string(getLitID(op2)) + ")";
                } else {
                    if (!presentST(op2))
                        ST[scnt++] = {scnt, op2, "0"};
                    IC += "(S,0" + to_string(getSymID(op2)) + ")";
                }
            }
            LC++;
        }

        cout << "  " << label << "\t\t" << opcode << "\t\t" << op1 << "\t\t" << op2
             << "\t" << lc << "\t\t" << IC << endl;
        ic << lc << "\t" << IC << endl;
    }

    // Output tables
    cout << "\n----------------------------------------------------------------\n";
    cout << " -- SYMBOL TABLE --\n\n<NO.\tSYMBOL\tADDRESS>\n";
    for (int i = 0; i < scnt; ++i) {
        cout << "  " << ST[i].no << "\t " << ST[i].sname << "\t  " << ST[i].addr << endl;
        st << ST[i].no << "\t " << ST[i].sname << "\t  " << ST[i].addr << endl;
    }

    cout << "\n----------------------------------------------------------------\n";
    cout << " -- LITERAL TABLE --\n\n<NO.\tLITERAL\tADDRESS>\n";
    for (int i = 0; i < lcnt; ++i) {
        cout << "  " << LT[i].no << "\t " << LT[i].lname << "\t  " << LT[i].addr << endl;
        lt << LT[i].no << "\t " << LT[i].lname << "\t  " << LT[i].addr << endl;
    }

    cout << "\n----------------------------------------------------------------\n";
    cout << " -- POOL TABLE --\n\n<NO.\tLITERAL_NO.>\n";
    for (int i = 0; i < pcnt; ++i)
        cout << "  " << PT[i].no << "\t   " << PT[i].lno << endl;

    return 0;
}