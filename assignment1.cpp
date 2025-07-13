#include <iostream>
#include <queue>
#include <thread>
#include <chrono>
using namespace std;

class Semaphore {
public:
    int val;
    Semaphore(int v) { val = v; }
    void waitS() {
        while (val <= 0) this_thread::yield();
        val--;
    }
    void signalS() {
        val++;
    }
};

class ProducerConsumer {
public:
    int maxSize;
    queue<int> buffer;
    Semaphore E, F, S;

    ProducerConsumer(int size) : E(size), F(0), S(1) {
        maxSize = size;
    }

    void produce(int item) {
        if (E.val == 0)
            cout << "[WAIT] Buffer full. Producer waiting...\n";

        E.waitS();
        S.waitS();

        buffer.push(item);
        cout << "Produced: " << item << " | Buffer Size: " << buffer.size() << endl;

        S.signalS();
        F.signalS();
    }

    void consume() {
        if (F.val == 0)
            cout << "[WAIT] Buffer empty. Consumer waiting...\n";

        F.waitS();
        S.waitS();

        int item = buffer.front();
        buffer.pop();
        cout << "Consumed: " << item << " | Buffer Size: " << buffer.size() << endl;

        S.signalS();
        E.signalS();
    }
};

void producerThread(ProducerConsumer &pc, int n) {
    for (int i = 1; i <= n; i++) {
        pc.produce(i);
        this_thread::sleep_for(chrono::milliseconds(100)); 
    }
}

void consumerThread(ProducerConsumer &pc, int n) {
    for (int i = 1; i <= n; i++) {
        pc.consume();
        this_thread::sleep_for(chrono::milliseconds(300));
    }
}

int main() {
    int bufferSize, totalItems;
    cout << "Enter buffer size: ";
    cin >> bufferSize;
    cout << "Enter how many items to produce/consume: ";
    cin >> totalItems;

    ProducerConsumer pc(bufferSize);

    thread t1(producerThread, ref(pc), totalItems);
    thread t2(consumerThread, ref(pc), totalItems);

    t1.join();
    t2.join();

    return 0;
}
