/* Program implementing the classic Dining Philosophers problem
 *  Written by Peter Strazdins RSCS ANU 08/15 for CISRA workshop
 */

#include <stdio.h>

#include <condition_variable>
#include <mutex>
#include <thread>

class Fork {  // monitor to model fork

 private:
  bool taken;  // monitor state; available if false
  int forkId;  // position of fork in the ring
  std::mutex forkLock;
  std::condition_variable isTaken;

 public:
  Fork(int id) {
    forkId = id;
    taken = false;
  }

  void get(int pId) {
    std::unique_lock<std::mutex> l(forkLock);
    isTaken.wait(l, [this]() { return (taken == false); });
    taken = true;
    printf("phil[%d].fork[%d].get\n", pId, forkId);
  }

  void put(int pId) {
    std::unique_lock<std::mutex> l(forkLock);
    taken = false;
    printf("phil[%d].fork[%d].get\n", pId, forkId);
    isTaken.notify_one();
  }
};  // Fork()

const int NumOps = 20;

void philosopher(int id, Fork* left, Fork* right) {
  while (true) {
    printf("phil[%d].sitdown\n", id);
    left->get(id);
    right->get(id);
    printf("phil[%d].eat\n", id);
    left->put(id);
    right->put(id);
    printf("phil[%d].arise\n", id);
  }
}

int main(int argc, char* argv[]) {
  int N = (argc > 1) ? atoi(argv[1]) : 5;
  printf("Dining Philosophers with %d philosophers", N);

  Fork** fork = new Fork*[N];
  for (int i = 0; i < N; i++) {
    fork[i] = new Fork(i);
  }

  std::thread** phil = new std::thread*[N];
  for (int i = 0; i < N; i++) {
    phil[i] = new std::thread(philosopher, i, fork[(i + N - 1) % N], fork[i]);
  }

  for (int i = 0; i < N; i++) phil[i]->join();

  for (int i = 0; i < N; i++) {
    delete phil[i];
    delete fork[i];
  }
  delete[] phil;
  delete[] fork;
  return 0;
}  // main()
