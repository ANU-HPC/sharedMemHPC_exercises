#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#define ASIZE 10

void cpu_time(double *tcpu);

int main(int argc, char* argv) {
  double time[ASIZE], delta; 
  int i, count0, count2;
  double tcpu0, tcpu1;
  count0 = 0, count2 = 0;
  while (count2 < ASIZE) {
    cpu_time(&tcpu0);
    cpu_time(&tcpu1);
    delta = tcpu1 - tcpu0;
    if (delta == 0) count0++;
    else if (delta > 0) time[count2] = delta, count2++;
  }
  printf("Total timings %d \n", count0 + count2);
  printf("Number of  0 delta time %d \n", count0);
  printf("Number of >0 delta time %d and values (in sec):\n", count2);
  for (i=0; i<ASIZE; i++) {
    printf("%e\t", time[i]);
    if (i%10 == 9) printf("\n");
  }
  printf("\n");
  return 0;
} //main()

void cpu_time(double *tcpu) {
    struct tms timestruct;
    long sec0, sec1;

    static long tick = 0;
    if(!tick)tick = sysconf(_SC_CLK_TCK);

    times(&timestruct);

    sec0 = timestruct.tms_utime/tick;
    sec1 = timestruct.tms_utime - sec0*tick;
    *tcpu= (double) sec0 + (double) sec1 / (double) tick;
}

