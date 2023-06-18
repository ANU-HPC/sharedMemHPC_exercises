/* Program implementing an integer bounded buffer monitor.
 *  Written  by Peter Strazdins RSCS ANU 08/15 for CISRA workshop
 */

#include <stdio.h>

#include <condition_variable>
#include <mutex>
#include <thread>

class BufferMon {
 private:
  int in, out, count;  // count = (in - out + N) % N;
  int N;
  int *buf;

  std::mutex bufLock;
  std::condition_variable notFull, notEmpty;

 public:
  BufferMon(int capacity) {
    N = capacity;
    count = 0;
    buf = new int[N];
    in = out = 0;
  }

  ~BufferMon() { delete[] buf; }

  int get() {
    std::unique_lock<std::mutex> l(bufLock);
    notEmpty.wait(l, [this]() { return count > 0; });
    int v = buf[out];
    buf[out] = -1;
    out = (out + 1) % N;
    count--;
    printf("get %d\n", v);
    notFull.notify_one();
    return v;
  }

  void put(int v) {
    std::unique_lock<std::mutex> l(bufLock);
    notFull.wait(l, [this]() { return count < N; });
    buf[in] = v;
    in = (in + 1) % N;
    count++;
    printf("put %d\n", v);
    notEmpty.notify_one();
  }
};

const int NumOps = 20;

void consumer(BufferMon &buf) {
  for (int i = 0; i < NumOps; i++) int v = buf.get();
}

void producer(BufferMon &buf) {
  for (int i = 0; i < NumOps; i++) buf.put(i);
}

int defBufSize = 5;
int main(int argc, char *argv[]) {
  int N = (argc > 1) ? atoi(argv[1]) : defBufSize;
  BufferMon buf(N);
  printf("Bounded buffer of size %d running for %d operations\n", N, NumOps);

  std::thread cons(consumer, std::ref(buf));
  std::thread prod(producer, std::ref(buf));

  prod.join();
  cons.join();
}
