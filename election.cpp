#include<bits/stdc++.h>
using namespace std;
void ringElection(const vector<int> &processes, const vector<bool> &alive, int initiator)
{
    int n = processes.size();
    int startIdx = -1;

    // Find the index of initiator
    for (int i = 0; i < n; ++i)
        if (processes[i] == initiator)
            startIdx = i;

    if (startIdx == -1 || !alive[startIdx])
    {
        cout << "Initiator is not alive or does not exist. Cannot start election.\n";
        return;
    }

    cout << "\n--- Ring Election Started ---\n";
    vector<int> token;
    int i = startIdx;

    // Pass the token around the ring once
    do
    {
        if (alive[i])
        {
            cout << "Process " << processes[i] << " adds its ID to token.\n";
            token.push_back(processes[i]);
        }
        else
        {
            cout << "Process " << processes[i] << " is DOWN, token skipped.\n";
        }
        i = (i + 1) % n;
    } while (i != startIdx);

    // Highest ID becomes coordinator
    cout << "\nToken contains IDs: ";
    for (int id : token)
        cout << id << " ";
    cout << endl;

    int coordinator = *max_element(token.begin(), token.end());
    cout << "\nCoordinator selected: " << coordinator << endl;

    // Announce the coordinator
    cout << "\n--- Announcing Coordinator ---\n";
    i = startIdx;
    do
    {
        if (alive[i])
            cout << "Process " << processes[i] << " acknowledges coordinator " << coordinator << endl;
        i = (i + 1) % n;
    } while (i != startIdx);

    cout << "--- Ring Election Ended ---\n";
}

void bullyElection(const vector<int> &processes, const vector<bool> &alive, int initiator)
{
    int n = processes.size();
    int startIdx = -1;

    // Find initiator index
    for (int i = 0; i < n; ++i)
        if (processes[i] == initiator)
            startIdx = i;

    if (startIdx == -1 || !alive[startIdx])
    {
        cout << "Initiator is not alive or does not exist. Cannot start election.\n";
        return;
    }

    cout << "\n--- Bully Election Started ---\n";
    cout << "Process " << initiator << " detects that coordinator is down.\n";

    int current = initiator;
    int coordinator = -1;

    // Everything inside one continuous loop
    while (true)
    {
        cout << "\nProcess " << current << " initiates an election.\n";
        bool higherFound = false;
        int next = -1;

        // Send election message to all higher processes
        for (int i = 0; i < n; ++i)
        {
            if (processes[i] > current)
            {
                cout << "  Process " << current << " -> sends ELECTION to Process " << processes[i];
                if (alive[i])
                {
                    cout << " -> ALIVE (Responds OK)\n";
                    higherFound = true;
                    next = processes[i]; // first higher alive process takes over
                    break;               // stop after first alive higher process
                }
                else
                {
                    cout << " -> DOWN\n";
                }
            }
        }

        if (!higherFound)
        {
            coordinator = current;
            break; // no higher alive found, so this one becomes coordinator
        }

        // Pass control to next higher alive process
        current = next;
    }

    cout << "\n=> Process " << coordinator << " becomes the new COORDINATOR.\n";

    // Announce the coordinator
    cout << "\n--- Announcing COORDINATOR ---\n";
    for (int i = 0; i < n; ++i)
    {
        if (alive[i])
            cout << "Process " << processes[i] << " acknowledges Coordinator " << coordinator << endl;
        else
            cout << "Process " << processes[i] << " is DOWN and misses the announcement.\n";
    }

    cout << "--- Bully Election Ended ---\n";
}
int main()
{
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<int> processes(n);
    vector<bool> alive(n);

    cout << "Enter process IDs (space separated): ";
    for (int i = 0; i < n; ++i)
        cin >> processes[i];

    cout << "Enter status for each process (1 = alive, 0 = down): ";
    for (int i = 0; i < n; ++i)
    {
        int s;
        cin >> s;
        alive[i] = (s == 1);
    }

    int initiator;
    cout << "Enter initiator process ID: ";
    cin >> initiator;

    cout << "\nSelect Election Algorithm:\n1. Ring Election\n2. Bully Election\n";
    int choice;
    cin >> choice;

    if (choice == 1)
        ringElection(processes, alive, initiator);
    else if (choice == 2)
        bullyElection(processes, alive, initiator);
    else
        cout << "Invalid choice.\n";

    return 0;
}
