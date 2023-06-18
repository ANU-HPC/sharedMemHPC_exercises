#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>  //gettimeofday()
#define MAX_THREADS 512

void *compute_pi(void *);
int num_threads, num_points;

int main(int argc, char *argv[]) {
  int i, hits[MAX_THREADS], total_hits;
  double pi_estimate, delta;
  struct timeval tp1, tp2;
  pthread_t p_threads[MAX_THREADS];
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  if (argc != 3) {
    printf(" %s num_threads num_points \n", argv[0]);
    return -1;
  } else {
    num_threads = atol(argv[1]);
    num_points = atol(argv[2]);
    if (num_threads > MAX_THREADS) num_threads = MAX_THREADS;
  }

  printf("Pthread Monte Carlo code to compute Pi\n");
  printf("num_threads: %d\n", num_threads);
  printf("num_points:  %d\n", num_points);

  gettimeofday(&tp1, NULL);
  for (i = 0; i < num_threads; i++) {
    hits[i] = i;  // WARNING: passing the effective thread id AND the address
                  // of the thread's hit counter in a single argument &hits[i]
    pthread_create(&p_threads[i], &attr, compute_pi, (void *)&hits[i]);
  }
  for (i = 0; i < num_threads; i++) {
    pthread_join(p_threads[i], NULL);
    total_hits += hits[i];
  }
  gettimeofday(&tp2, NULL);
  delta = (double)(tp2.tv_sec - tp1.tv_sec) +
          (double)(tp2.tv_usec - tp1.tv_usec) * 1.0e-6;

  pi_estimate = 4.0 * (double)total_hits / (double)num_points;
  printf("num_hits:    %12d\n", total_hits);
  printf("pi_estimate: %12.6f\n", pi_estimate);
  printf("time taken:  %12.6f\n", delta);
}  // main()

void *compute_pi(void *s) {
  struct drand48_data randBuffer;
  int i, my_id, *hit_pointer, local_hits;
  double rand_no_x, rand_no_y;
  hit_pointer = (int *)s;
  my_id = *hit_pointer;
  *hit_pointer = 0;  // zero out the id originally stored in *hit_pointer
  // ensure each thread has a different random number sequence
  srand48_r((long)my_id, &randBuffer);
  local_hits = 0;
  for (i = my_id; i < num_points; i += num_threads) {
    drand48_r(&randBuffer, &rand_no_x);
    drand48_r(&randBuffer, &rand_no_y);
    if (((rand_no_x - 0.5) * (rand_no_x - 0.5) +
         (rand_no_y - 0.5) * (rand_no_y - 0.5)) < 0.25)
      *hit_pointer = *hit_pointer + 1;  // comment to use local variable
    // local_hits++;           //uncomment to use local variable
  }
//  *hit_pointer = local_hits;  //uncomment to use local variable
  pthread_exit(0);
}  // compute_pi()
