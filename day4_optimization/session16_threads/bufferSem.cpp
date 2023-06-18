/* Program implementing an integer bounded buffer via semaphores
 * Written  by Peter Strazdins RSCS ANU 08/15 for CISRA workshop
 */

#include <stdio.h>

#include <condition_variable>
#include <mutex>
#include <thread>

class Semaphore {
 private:
  int v;
  std::mutex semLock;
  std::condition_variable semZero;

 public:
  Semaphore(int initial) { v = initial; }

  void up() {
    std::unique_lock<std::mutex> l(semLock);
    v++;
    semZero.notify_all();
  }

  void down() {
    std::unique_lock<std::mutex> l(semLock);
    semZero.wait(l, [this]() { return v > 0; });
    v--;
  }

};  // Semaphore

class BufferSem {
 private:
  int in, out, count;  // count = (in -out + N) % N;
  int N;
  int *buf;
  std::mutex bufLock;
  Semaphore *full, *empty;

 public:
  BufferSem(int capacity) {
    N = capacity;
    count = 0;
    buf = new int[N];
    in = out = 0;
    full = new Semaphore(0);
    empty = new Semaphore(N);
  }

  ~BufferSem() {
    delete buf;
    delete full;
    delete empty;
  }

  int get() {
    full->down();
    std::unique_lock<std::mutex> l(bufLock);
    int v = buf[out];
    buf[out] = -1;
    out = (out + 1) % N;
    count--;
    printf("get %d\n", v);
    empty->up();
    return v;
  }

  void put(int v) {
    empty->down();
    std::unique_lock<std::mutex> l(bufLock);
    buf[in] = v;
    in = (in + 1) % N;
    count++;
    printf("put %d\n", v);
    full->up();
  }
};

const int NumOps = 20;

void consumer(BufferSem &buf) {
  for (int i = 0; i < NumOps; i++) int v = buf.get();
}

void producer(BufferSem &buf) {
  for (int i = 0; i < NumOps; i++) buf.put(i);
}

int defBufSize = 5;
int main(int argc, char *argv[]) {
  int N = (argc > 1) ? atoi(argv[1]) : defBufSize;
  BufferSem buf(N);
  printf("Bounded buffer of size %d running for %d operations\n", N, NumOps);

  std::thread cons(consumer, std::ref(buf));
  std::thread prod(producer, std::ref(buf));

  prod.join();
  cons.join();
}
