#include <stdio.h>

#include <iostream>
#include <thread>

void hello(int i) { printf("Hello from thread logical id %d\n", i); }

const int N = 5;
const int R = 1000;

class Counter {
 public:
  int v;
  Counter() { v = 0; }
  void inc() { v++; }
};

int main() {
  Counter count;
  std::thread** ts = new std::thread*[N];

  for (int i = 0; i < N; i++)
    ts[i] = new std::thread([&count]() {
      for (int j = 0; j < R; j++) count.inc();
    });

  for (int i = 0; i < N; i++) ts[i]->join();

  printf("after %d increments by %d threads, the counter is %d\n", R, N,
         count.v);
}
