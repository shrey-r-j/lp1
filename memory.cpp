// #include <bits/stdc++.h>
#include <iostream>
#include <list>
#include<algorithm>
using namespace std;

struct node{
    bool occupied;
    int cap;
    string name;
    node(int c){
        cap = c;
        occupied = false;
        name = "";
    }
};

list<node> memory;
list<node>::iterator lastPos;

void occupy(int pos,int v,string name){
    auto it = memory.begin();
    advance(it,pos);
    int c = it->cap;
    if(it->cap > v){
        it->occupied = true;
        it->cap = v;
        it->name = name;
        advance(it,1);
        node temp(c-v);
        memory.insert(it,temp);
    }    
    else{
        it->occupied = true;
        it->cap = v;
        it->name = name;
    }
    lastPos = it;
}

void firstfit(int v,string name){
    int c = 0;
    for(auto it:memory){
        if(it.occupied==false && it.cap >= v){
            break;
        }
        c++;
    }
    if(c < (int)memory.size()) occupy(c,v,name);
    else{
        cout<<"No block available"<<endl;
    }
}

void bestfit(int v,string name){
    int bestpos = -1,pos = 0, bestcap = INT_MAX;
    for(auto it:memory){
        if(!it.occupied && it.cap >= v && it.cap < bestcap){
            bestpos = pos;
            bestcap = it.cap;
        }
        pos++;
    }
    if(bestpos !=-1) occupy(bestpos,v,name);
    else cout<<"No block available"<<endl;
}

void worstfit(int v,string name){
    int worstpos = -1, pos = 0, worstcap = INT_MIN;
    for(auto it:memory){
        if(!it.occupied && it.cap >= v && worstcap < it.cap){
            worstpos = pos;
            worstcap = it.cap;
        }
        pos++;
    }
    if(worstpos!=-1){
        occupy(worstpos,v,name);
    }
    else cout<<"No block available"<<endl;

}

void nextfit(int v,string name){
    if(memory.empty()){
        cout<<"No block available"<<endl;
        return;
    }
   
    auto start = lastPos;
    auto i = lastPos;
    do{
        if(!i->occupied && i->cap >=v){
            int pos = distance(memory.begin(),i);
            occupy(pos,v,name);
            return;
        }
        i++;
        if(i==memory.end()) i = memory.begin();
    }
    while(i!=start);
    cout<<"No block available"<<endl;
}

void show(){
    for(auto it:memory){
        if(!it.occupied){
            cout<<"["<<it.cap<<"] ";
        }
        else{
            cout<<"["<<it.name<<" : "<<it.cap<<"] ";
        }
    }
    cout<<endl;
}

int main(){
    
    int n;
    cout<<"Enter no of blocks ";
    cin>>n;

    for(int i=0;i<n;i++){
        int x;
        cin>>x;
        node temp(x);
        memory.push_back(x);
    }

    lastPos = memory.begin();

    while(true){
        cout<<"\nMenu";
        cout<<"1. First Fit"<<endl;
        cout<<"2. Best Fit"<<endl;
        cout<<"3. Worst Fit"<<endl;
        cout<<"4. Next Fit"<<endl;
        cout<<"5. Show Memory"<<endl;
        cout<<"6. Exit"<<endl;
        cout<<"Enter choice ";
        int ch;
        cin>>ch;
        if(ch == 1){
            cout<<"Enter name ";
            string name;
            cin>>name;
            cout<<"Enter capacity ";
            int v;
            cin>>v;
            firstfit(v,name);
        }
        else if(ch == 2){
            cout<<"Enter name ";
            string name;
            cin>>name;
            cout<<"Enter capacity ";
            int v;
            cin>>v;
            bestfit(v,name);
        }
        else if(ch == 4){
            cout<<"Enter name ";
            string name;
            cin>>name;
            cout<<"Enter capacity ";
            int v;
            cin>>v;
            nextfit(v,name);
        }
        else if(ch == 3){
            cout<<"Enter name ";
            string name;
            cin>>name;
            cout<<"Enter capacity ";
            int v;
            cin>>v;
            worstfit(v,name);
        }
        else if(ch == 5){
            show();
        }
        else{
            break;
        }
    }
    return 0;
}