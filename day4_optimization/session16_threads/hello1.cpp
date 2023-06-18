#include <stdio.h>

#include <iostream>
#include <thread>
void hello(int i) { printf("Hello from thread logical id %d\n", i); }

const int N = 5;

int main() {
  std::thread** ts = new std::thread*[N];
  for (int i = 0; i < N; i++) ts[i] = new std::thread(hello, i);
  for (int i = 0; i < N; i++) ts[i]->join();
}
