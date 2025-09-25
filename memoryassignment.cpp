#include <bits/stdc++.h>
using namespace std;

// Represents a memory block (free block)
struct Block {
    int size;
    int index;
    bool allocated = false;
};

// Represents a process needing memory
struct Process {
    int id;
    int memory;
    int allocatedBlockPos = -1;  // Store the position in blocks vector
};

// First Fit Allocation
void firstFit(vector<Block>& blocks, vector<Process>& processes) {
    cout << "\nFirst Fit Allocation:\n";
    for (auto& p : processes) {
        p.allocatedBlockPos = -1;
        for (int i = 0; i < (int)blocks.size(); ++i) {
            if (!blocks[i].allocated && blocks[i].size >= p.memory) {
                p.allocatedBlockPos = i;
                blocks[i].allocated = true;
                cout << "Process " << p.id << " allocated to Block of size " << blocks[i].size << "\n";
                break;
            }
        }
        if (p.allocatedBlockPos == -1) {
            cout << "Process " << p.id << " not allocated\n";
        }
    }
}

// Next Fit Allocation (no wrap-around)
void nextFit(vector<Block>& blocks, vector<Process>& processes) {
    cout << "\nNext Fit (No Wrap-Around) Allocation:\n";
    int n = blocks.size();
    int startIndex = 0;

    for (auto& p : processes) {
        p.allocatedBlockPos = -1;
        bool allocated = false;

        for (int i = startIndex; i < n; i++) {
            if (!blocks[i].allocated && blocks[i].size >= p.memory) {
                p.allocatedBlockPos = i;
                blocks[i].allocated = true;
                startIndex = i + 1;
                allocated = true;
                cout << "Process " << p.id << " allocated to Block of size " << blocks[i].size << "\n";
                break;
            }
        }

        if (!allocated) {
            cout << "Process " << p.id << " not allocated\n";
        }
    }
}

// Best Fit Allocation
void bestFit(vector<Block>& blocks, vector<Process>& processes) {
    cout << "\nBest Fit Allocation:\n";
    for (auto& p : processes) {
        p.allocatedBlockPos = -1;
        int bestBlockPos = -1;
        int bestSize = INT_MAX;

        for (int i = 0; i < (int)blocks.size(); ++i) {
            if (!blocks[i].allocated && blocks[i].size >= p.memory && blocks[i].size < bestSize) {
                bestSize = blocks[i].size;
                bestBlockPos = i;
            }
        }

        if (bestBlockPos != -1) {
            blocks[bestBlockPos].allocated = true;
            p.allocatedBlockPos = bestBlockPos;
            cout << "Process " << p.id << " allocated to Block of size " << blocks[bestBlockPos].size << "\n";
        } else {
            cout << "Process " << p.id << " not allocated\n";
        }
    }
}

// Worst Fit Allocation
void worstFit(vector<Block>& blocks, vector<Process>& processes) {
    cout << "\nWorst Fit Allocation:\n";
    for (auto& p : processes) {
        p.allocatedBlockPos = -1;
        int worstBlockPos = -1;
        int worstSize = -1;

        for (int i = 0; i < (int)blocks.size(); ++i) {
            if (!blocks[i].allocated && blocks[i].size >= p.memory && blocks[i].size > worstSize) {
                worstSize = blocks[i].size;
                worstBlockPos = i;
            }
        }

        if (worstBlockPos != -1) {
            blocks[worstBlockPos].allocated = true;
            p.allocatedBlockPos = worstBlockPos;
            cout << "Process " << p.id << " allocated to Block of size " << blocks[worstBlockPos].size << "\n";
        } else {
            cout << "Process " << p.id << " not allocated\n";
        }
    }
}

int main() {
    int numBlocks, numProcesses;

    // Input memory blocks
    cout << "Enter number of memory blocks: ";
    cin >> numBlocks;
    vector<Block> blocks(numBlocks);

    cout << "Enter sizes of each block:\n";
    for (int i = 0; i < numBlocks; ++i) {
        cin >> blocks[i].size;
        blocks[i].index = i;
        blocks[i].allocated = false;
    }

    // Input processes
    cout << "\nEnter number of processes: ";
    cin >> numProcesses;
    vector<Process> processes(numProcesses);

    cout << "Enter memory requirement for each process:\n";
    for (int i = 0; i < numProcesses; ++i) {
        processes[i].id = i + 1;
        cin >> processes[i].memory;
    }

    // Make copies for each allocation strategy
    auto blocksFF = blocks;
    auto blocksNF = blocks;
    auto blocksBF = blocks;
    auto blocksWF = blocks;

    auto processesFF = processes;
    auto processesNF = processes;
    auto processesBF = processes;
    auto processesWF = processes;

    // Perform allocations
    firstFit(blocksFF, processesFF);
    nextFit(blocksNF, processesNF);
    bestFit(blocksBF, processesBF);
    worstFit(blocksWF, processesWF);

    return 0;
}
