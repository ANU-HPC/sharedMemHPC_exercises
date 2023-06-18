#include <stdio.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
int main(int argc, char **argv) {
  struct tms cpu;
  struct timeval tp1, tp2;
  struct timezone tzp;
  gettimeofday(&tp1, NULL);
  long tick = sysconf(_SC_CLK_TCK);
  sleep(1);
  printf(" Ticks per second %ld \n", tick);
  gettimeofday(&tp2, NULL);
  times(&cpu);
  printf(" User   ticks %d \n", cpu.tms_utime);
  printf(" System ticks %d \n", cpu.tms_stime);
  printf(" Elapsed secs %d usec %d \n", tp2.tv_sec - tp1.tv_sec,
         tp2.tv_usec - tp1.tv_usec);
}
