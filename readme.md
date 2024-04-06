# Assignment 3
## Part 1
To run: g++ -O3 Part1.cpp && ./a.out > out.txt

In the minotaur's original strategy, there was likely a race condition or similar bug causing presents not to be deleted properly after writing thank you notes. In my design, I chose to use a fine-grained concurrent linked list, with a lock on each individual node to protect against race conditions. This is much faster than locking the entire list (which essentially is just single threaded), as it allows multiple threads to act on the linked list at the same time as long as they are in different locations. Runs relatively quickly (<20s).

## Part 2
To run: g++ -O3 Part2.cpp && ./a.out > out.txt

To simulate in a reasonable amount of time, I sped up the time by a factor of 60, using seconds instead of minutes. This makes no difference on the implementation details. Since we essentially only need to store 10 values (5 high and 5 low), we can simply use an array that only stores those values. Since we are doing very few operations and only have 8 threads, we can just lock the entire array while doing updates instead of using a concurrent data structure. The very fast O(1) updates are almost negligible compared to the interval delays, which means that we will never miss an interval update because of waiting on the mutex (all 8 threads will take far less than 1s to update). The main thread can simply take the difference between the min and max temperature every ten seconds, and print the results every minute.