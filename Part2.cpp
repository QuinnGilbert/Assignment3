#include <bits/stdc++.h>
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;

/*
Using second and minutes instead of minutes and hours, the code is essentially the same
*/

int main() {

    int numThreads = 8;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(-100, 70);

    // store 5 highest and lowest temperatures for every hour, and highest interval for every ten minutes
    vector<int> high(5);
    vector<int> low(5);
    vector<int> interval(6);
    mutex mtx;

    vector<thread> sensors(numThreads);
    for(int i=0;i<numThreads;i++) {
        sensors[i] = thread([&]() {
            auto start = chrono::high_resolution_clock::now();

            while(true) {
                auto current = chrono::high_resolution_clock::now();
                auto elapsed = chrono::duration_cast<chrono::seconds>(current - start).count();
                if(elapsed >= 1) {
                    // generate random temperature and store it if it is one of the current 5 highest
                    start = current;
                    int temp = dis(gen);

                    string output = "Record "+to_string(temp)+"\n";
                    cout<<output;

                    mtx.lock();
                    high.push_back(temp);
                    low.push_back(temp);
                    sort(begin(high), end(high));
                    sort(begin(low), end(low));
                    reverse(begin(high), end(high));
                    if(high.size() > 5) high.pop_back();
                    if(low.size() > 5) low.pop_back();
                    mtx.unlock();
                }
            }     

        });
    }

    auto start = chrono::high_resolution_clock::now();
    int count=0;

    while(true) {
        auto current = chrono::high_resolution_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(current - start).count();
        if(elapsed >= 10) {
            // store the highest difference for each interval of 10 seconds
            start = current;

            mtx.lock();
            interval[count] = high[0] - low[0];
            mtx.unlock();

            count++;
            if(count==6) {
                // Give the report and clear the stored values
                mtx.lock();
                count = 0;
                string result = "Highest: ";
                for(int i=0;i<5;i++) {
                    result += to_string(high[i])+" ";
                }
                result += "\nLowest: ";
                for(int i=0;i<5;i++) {
                    result += to_string(low[i])+" ";
                }
                int maxi = 0;
                for(int i=0;i<6;i++) {
                    if(interval[i] > interval[maxi]) maxi = i;
                }
                result += "\nGreatest interval occurred between minute "+to_string(maxi*10)+" and "+to_string((maxi+1)*10)+"\n";
                cout<<result;
                high.clear();
                low.clear();
                mtx.unlock();
            }

        }
    }     


}