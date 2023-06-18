#include <stdio.h>

#include <thread>

const int N = 5;
int main() {
  std::thread** ts = new std::thread*[N];
  for (int i = 0; i < N; i++)
    ts[i] = new std::thread([&i]() { printf("Hello from thread %d\n", i); });
  for (int i = 0; i < N; i++) ts[i]->join();
}
