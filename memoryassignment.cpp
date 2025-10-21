#include <bits/stdc++.h>
using namespace std;

struct Block {
    int size;
    int index;
    bool allocated = false;
};

struct Process {
    int id;
    int memory;
    int allocated = -1;
};

void firstfit(vector<Block> &Blocks, vector<Process> &Processes) {
    for (int i = 0; i < Processes.size(); i++) {
        Processes[i].allocated = -1;
        for (int j = 0; j < Blocks.size(); j++) {
            if (!Blocks[j].allocated && Blocks[j].size >= Processes[i].memory) {
                Processes[i].allocated = j;
                Blocks[j].allocated = true;
                cout << "Process " << Processes[i].id << " -> Block " << Blocks[j].index << " (" << Blocks[j].size << ")" << endl;
                break;
            }
        }
        if (Processes[i].allocated == -1)
            cout << "Process " << Processes[i].id << " Unallocated" << endl;
    }
}

void nextfit(vector<Block> &Blocks, vector<Process> &Processes) {
    int startindex = 0;
    for (int i = 0; i < Processes.size(); i++) {
        Processes[i].allocated = -1;
        for (int j = startindex; j < Blocks.size(); j++) {
            if (!Blocks[j].allocated && Blocks[j].size >= Processes[i].memory) {
                Processes[i].allocated = j;
                Blocks[j].allocated = true;
                startindex = j + 1;
                cout << "Process " << Processes[i].id << " -> Block " << Blocks[j].index << " (" << Blocks[j].size << ")" << endl;
                break;
            }
        }
        if (Processes[i].allocated == -1)
            cout << "Process " << Processes[i].id << " Unallocated" << endl;
    }
}

void bestfit(vector<Block> &Blocks, vector<Process> &Processes) {
    for (int i = 0; i < Processes.size(); i++) {
        int bestblockpos = -1, bestsize = INT_MAX;
        Processes[i].allocated = -1;
        for (int j = 0; j < Blocks.size(); j++) {
            if (!Blocks[j].allocated && Blocks[j].size >= Processes[i].memory && Blocks[j].size < bestsize) {
                bestblockpos = j;
                bestsize = Blocks[j].size;
            }
        }
        if (bestblockpos != -1) {
            Blocks[bestblockpos].allocated = true;
            Processes[i].allocated = bestblockpos;
            cout << "Process " << Processes[i].id << " -> Block " << Blocks[bestblockpos].index << " (" << Blocks[bestblockpos].size << ")" << endl;
        } else
            cout << "Process " << Processes[i].id << " Unallocated" << endl;
    }
}

void worstfit(vector<Block> &Blocks, vector<Process> &Processes) {
    for (int i = 0; i < Processes.size(); i++) {
        int worstblockpos = -1, worstsize = INT_MIN;
        Processes[i].allocated = -1;
        for (int j = 0; j < Blocks.size(); j++) {
            if (!Blocks[j].allocated && Blocks[j].size >= Processes[i].memory && Blocks[j].size > worstsize) {
                worstblockpos = j;
                worstsize = Blocks[j].size;
            }
        }
        if (worstblockpos != -1) {
            Blocks[worstblockpos].allocated = true;
            Processes[i].allocated = worstblockpos;
            cout << "Process " << Processes[i].id << " -> Block " << Blocks[worstblockpos].index << " (" << Blocks[worstblockpos].size << ")" << endl;
        } else
            cout << "Process " << Processes[i].id << " Unallocated" << endl;
    }
}

int main() {
    int nb, np;
    cout << "Enter number of blocks:" << endl;
    cin >> nb;
    vector<Block> blocks(nb);
    for (int i = 0; i < nb; i++) {
        cout << "Enter size of block " << i + 1 << ":" << endl;
        cin >> blocks[i].size;
        blocks[i].index = i + 1;
        blocks[i].allocated = false;
    }

    cout << "Enter number of processes:" << endl;
    cin >> np;
    vector<Process> processes(np);
    for (int i = 0; i < np; i++) {
        cout << "Enter memory required by process " << i + 1 << ":" << endl;
        cin >> processes[i].memory;
        processes[i].id = i + 1;
        processes[i].allocated = -1;
    }

    auto blocksFF = blocks, blocksNF = blocks, blocksBF = blocks, blocksWF = blocks;
    auto processesFF = processes, processesNF = processes, processesBF = processes, processesWF = processes;

    cout << "\nFirst Fit:" << endl;
    firstfit(blocksFF, processesFF);
    cout << "\nNext Fit:" << endl;
    nextfit(blocksNF, processesNF);
    cout << "\nBest Fit:" << endl;
    bestfit(blocksBF, processesBF);
    cout << "\nWorst Fit:" << endl;
    worstfit(blocksWF, processesWF);

    return 0;
}
