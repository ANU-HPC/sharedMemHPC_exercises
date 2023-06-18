#include <stdio.h>

#include <iostream>
#include <thread>

void hello() {
  std::cout << "Hello from thread " << std::this_thread::get_id() << std::endl;
}

int main() {
  std::thread t1(hello);
  t1.join();  // wait for t1 to finish
}
