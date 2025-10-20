#include<bits/stdc++.h>
using namespace std;
using namespace chrono;
using namespace this_thread;
class semaphore{
    public:
    int val;
    semaphore(int v){
        val=v;
    }
    void wait(){
        while(val<=0) yield();
        val--;
    }
    void signal(){
        val++;
    }
};
class ProducerConsumer{
    public:
    queue<int>buffer;
    semaphore e,f,s;
    int maxsize;
    ProducerConsumer(int size):e(size),f(0),s(1){
        maxsize=size;
    }

    void produce(int item){
        e.wait();
        s.wait();
        buffer.push(item);
        cout<<"Produced :"<<item<< "| Buffer Size :"<<buffer.size()<<endl;
        f.signal();
        s.signal();
    }
    void consume(){
        f.wait();
        s.wait();
        int item=buffer.front();
        buffer.pop();
        cout<<"Consumed :"<<item<<" |Buffer Size :"<<buffer.size()<<endl;
        e.signal();
        s.signal();
    }
};

void producer(ProducerConsumer & pc ,int n){
    for(int i=0;i<n;i++){
        pc.produce(i+1);
        sleep_for(milliseconds(100));
    }
}
void consumer(ProducerConsumer & pc ,int n){
    for(int i=0;i<n;i++){
        pc.consume();
        sleep_for(milliseconds(300));
    }
}

int main(){
    int buffersize;
    int total;
    cout<<"enter buffer size"<<endl;
    cin>>buffersize;
    cout<<"enter total items to produce"<<endl;
    cin>>total;
    ProducerConsumer pc(buffersize);
    thread t1(producer,ref(pc),total);
    thread t2(consumer,ref(pc),total);
    t1.join();
    t2.join();
    return 0;
}
