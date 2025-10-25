#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ----------- Ring Election -----------
void ringElection(const vector<int>& processes, const vector<bool>& alive, int initiator) {
    int n = processes.size();
    int startIdx = -1;
    for (int i = 0; i < n; ++i)
        if (processes[i] == initiator) startIdx = i;

    if (startIdx == -1 || !alive[startIdx]) {
        cout << "Initiator is not alive or does not exist. Cannot start election.\n";
        return;
    }

    cout << "\n--- Ring Election Started ---\n";
    vector<int> token;
    int i = startIdx;
    do {
        if (alive[i]) {
            cout << "Process " << processes[i] << " adds its ID to token.\n";
            token.push_back(processes[i]);
        } else {
            cout << "Process " << processes[i] << " is DOWN, token skipped.\n";
        }
        i = (i + 1) % n;
    } while (i != startIdx);

    cout << "\nToken contains IDs: ";
    for (int id : token) cout << id << " ";
    cout << endl;

    int coordinator = *max_element(token.begin(), token.end());
    cout << "\nCoordinator selected: " << coordinator << endl;

    cout << "\n--- Announcing Coordinator ---\n";
    i = startIdx;
    do {
        if (alive[i])
            cout << "Process " << processes[i] << " acknowledges coordinator " << coordinator << endl;
        i = (i + 1) % n;
    } while (i != startIdx);

    cout << "--- Ring Election Ended ---\n";
}

// ----------- Bully Election -----------
void bullyElection(const vector<int>& processes, const vector<bool>& alive, int initiator) {
    int n = processes.size();
    int starterIdx = -1;
    for (int i = 0; i < n; ++i)
        if (processes[i] == initiator) starterIdx = i;

    if (starterIdx == -1 || !alive[starterIdx]) {
        cout << "Starter is not alive or does not exist. Cannot start election.\n";
        return;
    }

    cout << "\n--- Bully Election Started ---\n";
    int currentId = initiator;
    int coordinator = -1;

    while (true) {
        cout << "\nProcess " << currentId << " starts election.\n";
        vector<int> higherResponders;

        for (int i = 0; i < n; ++i) {
            if (processes[i] > currentId) {
                cout << "Message sent to Process " << processes[i];
                if (alive[i]) {
                    cout << " (ALIVE, RESPONDS)";
                    higherResponders.push_back(processes[i]);
                } else {
                    cout << " (DOWN)";
                }
                cout << endl;
            }
        }

        if (higherResponders.empty()) {
            coordinator = currentId;
            break;
        } else {
            currentId = *max_element(higherResponders.begin(), higherResponders.end());
            cout << "Process " << currentId << " will take over election.\n";
        }
    }

    cout << "\nCoordinator selected: " << coordinator << endl;
    cout << "\n--- Announcing Coordinator ---\n";
    for (int i = 0; i < n; ++i) {
        if (alive[i])
            cout << "Process " << processes[i] << " acknowledges coordinator " << coordinator << endl;
        else
            cout << "Process " << processes[i] << " is DOWN and misses announcement.\n";
    }

    cout << "--- Bully Election Ended ---\n";
}

// ---------------- Main ----------------
int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<int> processes(n);
    vector<bool> alive(n);

    cout << "Enter process IDs (space separated): ";
    for (int i = 0; i < n; ++i) cin >> processes[i];

    cout << "Enter status for each process (1 = alive, 0 = down): ";
    for (int i = 0; i < n; ++i) {
        int s; cin >> s;
        alive[i] = (s == 1);
    }

    int initiator;
    cout << "Enter initiator process ID: ";
    cin >> initiator;

    cout << "\nSelect Election Algorithm:\n1. Ring Election\n2. Bully Election\n";
    int choice;
    cin >> choice;

    if (choice == 1) ringElection(processes, alive, initiator);
    else if (choice == 2) bullyElection(processes, alive, initiator);
    else cout << "Invalid choice.\n";

    return 0;
}
