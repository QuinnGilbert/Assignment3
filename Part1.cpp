#include <bits/stdc++.h>
#include <mutex>
#include <thread>

using namespace std;

struct Node {
    int tag;
    Node* next = nullptr;
    mutex node_mutex;
    Node(int tag) : tag(tag) {}
};

struct LinkedList {
    Node* head = new Node(-1);
    mutex list_mutex;
    
    // check whether a present (node) with a given tag exists
    bool check(int tag) {
        Node* cur = head;
        cur->node_mutex.lock();
        while(true) {
            if(cur->tag == tag) {
                cur->node_mutex.unlock();
                return true;
            }

            if(cur->next == nullptr) {
                cur->node_mutex.unlock();
                break;
            }
            cur->next->node_mutex.lock();
            Node* temp = cur;
            cur = cur->next;
            temp->node_mutex.unlock();
        }
        return false;
    }

     // add present with given tag into list in sorted order
    void add(int tag) {
        Node* cur = head;
        cur->node_mutex.lock();
        while(true) {
            if(cur->next == nullptr || cur->next->tag > tag) {

                Node* newNext =  new Node(tag);
                newNext->next = cur->next;
                cur->next = newNext;
                
                cur->node_mutex.unlock();
                return;
            }
            
            cur->next->node_mutex.lock();
            Node* temp = cur;
            cur = cur->next;
            temp->node_mutex.unlock();
        }
    }

    // erase the next present in the list
    int erase() {
        Node* cur = head;
        cur->node_mutex.lock();
        if (cur->next == nullptr) {
            cur->node_mutex.unlock();
            return -1;
        } else {
            Node* temp = cur->next;
            temp->node_mutex.lock();
            cur->next = cur->next->next;
            temp->node_mutex.unlock();
            cur->node_mutex.unlock();
            return temp->tag;
        }
    }

    int count() {
        int res = 0;
        Node* cur = head;
        while(cur->next != nullptr) {
            res++;
            cur = cur->next;
        }
        return res;
    }
};

int main() {

    int n = 500'000;
    int numThreads = 4;

    // generate bag with random order of elements
    vector<int> bag(n);
    for(int i=0;i<n;i++) {
        bag[i] = i+1;
    }
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine e(seed);
    shuffle(bag.begin(), bag.end(), e);

    int bagIndex = 0;
    mutex bag_mutex;
    vector<thread> servants(numThreads);
    LinkedList linkedList;
    for(int i=0;i<numThreads;i++) {
        servants[i] = thread([&](int id) {

            while(true) {
            
                int type = rand() % 3;
                // add the next present from the bag
                if(type == 0) {
                    bag_mutex.lock();
                    // if the bag is empty, erase instead
                    if(bagIndex == n) {
                        type = 1;
                        bag_mutex.unlock();
                    } else {
                        int tag = bag[bagIndex];
                        string output = "Add "+to_string(tag)+"\n";
                        cout<<output;
                        bagIndex++;
                        bag_mutex.unlock();
                        linkedList.add(tag);
                    }
                }
                if(type==1) {
                    int tag = linkedList.erase();
                    if(tag != -1) {
                        string output = "Write thank you to "+to_string(tag)+"\n";
                        cout<<output;
                    }
                    if(tag == -1 && bagIndex == n) break;
                }
                if(type==2) {
                    int tag = rand() % n + 1;
                    bool check = linkedList.check(tag);
                    if(check) {
                        string output = "Tag "+to_string(tag)+" is in the list\n";
                        cout << output;
                    } else {
                        string output = "Tag "+to_string(tag)+" is not in the list\n";
                        cout << output;
                    }
                }
            }
        }, i);
    }

    for(int i=0;i<numThreads;i++) {
        servants[i].join();
    }

}