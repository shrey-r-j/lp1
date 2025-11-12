#include <iostream>
#include <condition_variable>
#include <queue>
#include<thread>
// #include<vector>
using namespace std;
mutex mtx;
queue<int> buffer;
const int BUFFER_SIZE = 5;
condition_variable not_full,not_empty;

void print(){
    queue<int> tmp = buffer;
    cout<<"Buffer : [";
    while(!tmp.empty()){
        cout<<tmp.front()<<" ";
        tmp.pop();
    }
    cout<<"]"<<endl;
}

void producer(){
   while(true){ unique_lock<mutex> lock(mtx);
    not_full.wait(lock,[]{
        return buffer.size() < BUFFER_SIZE ;
    });
    int item = rand() % 100;
    buffer.push(item);
    cout<<"Produced :"<<item<<"\n";
    print();
    lock.unlock();
    not_empty.notify_one();
    this_thread::sleep_for(500ms);}
}

void consumer(){
    while(true){unique_lock<mutex> lock(mtx);
    not_empty.wait(lock,[]{
        return !buffer.empty();
    });
    int item = buffer.front();
    buffer.pop();
    cout<<"Consumed :"<<item<<"\n";
    print();
    lock.unlock();
    not_full.notify_one();
    this_thread::sleep_for(1s);}
}

int main(){
    thread p(producer);
    thread c(consumer);
    p.join();
    c.join();
    return 0;
}