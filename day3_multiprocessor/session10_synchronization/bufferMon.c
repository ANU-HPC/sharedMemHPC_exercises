/* Program implementing an integer bounded buffer monitor,.
 * Written  by Peter Strazdins RSCS ANU 08/15
 * To build: gcc -Wall -o bufferMon bufferMon.c -lpthread
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/************* bounded buffer monitor *************/
typedef struct bbuf_t {
  int count, N;
  int in, out;  // count = (in - out + N) % N;
  int *buf;

  pthread_mutex_t bufLock;
  pthread_cond_t notFull;
  pthread_cond_t notEmpty;
} bbuf_t;

void makeBB(int capacity, bbuf_t *b) {
  b->N = capacity;
  b->count = 0;
  b->buf = malloc(b->N * sizeof(int));
  b->in = b->out = 0;

  pthread_mutex_init(&(b->bufLock), NULL);
  pthread_cond_init(&(b->notFull), NULL);
  pthread_cond_init(&(b->notEmpty), NULL);
}  // makeBB()

void delBB(bbuf_t *b) {
  free(b->buf);
  b->buf = NULL;

  pthread_mutex_destroy(&(b->bufLock));
  pthread_cond_destroy(&(b->notEmpty));
  pthread_cond_destroy(&(b->notFull));
}  // delBB()

void putBB(int v, bbuf_t *b) {
  pthread_mutex_lock(&b->bufLock);
  while (b->count == b->N) pthread_cond_wait(&b->notFull, &b->bufLock);

  b->count++;
  b->buf[b->in] = v;
  b->in = (b->in + 1) % b->N;
  printf("put %d\n", v);

  pthread_cond_signal(&b->notEmpty);
  pthread_mutex_unlock(&b->bufLock);
}  // putBB()

int getBB(bbuf_t *b) {
  int v;
  pthread_mutex_lock(&b->bufLock);
  while (b->count == 0) pthread_cond_wait(&b->notEmpty, &b->bufLock);

  b->count--;
  v = b->buf[b->out];
  b->buf[b->out] = -1;
  b->out = (b->out + 1) % b->N;
  printf("get %d\n", v);

  pthread_cond_signal(&b->notFull);
  pthread_mutex_unlock(&b->bufLock);
  return v;
}  // getBB

/************ producer / consumer using bounded buffer ************/

const int NumOps = 20;
static bbuf_t buff;

void *consumer(void *args) {
  int i;
  for (i = 0; i < NumOps; i++) getBB(&buff);
  return NULL;
}

void *producer(void *args) {
  int i;
  for (i = 0; i < NumOps; i++) putBB(i, &buff);
  return NULL;
}

int defBufSize = 5;
int main(int argc, char *argv[]) {
  int N = (argc > 1) ? atoi(argv[1]) : defBufSize;
  pthread_t threadProd, threadCons;

  printf("Bounded buffer of size %d running for %d operations\n", N, NumOps);
  makeBB(N, &buff);

  pthread_create(&threadCons, NULL, consumer, NULL);
  pthread_create(&threadProd, NULL, producer, NULL);

  pthread_join(threadProd, NULL);
  pthread_join(threadCons, NULL);
  return 0;
}
