#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define ASIZE 10
int main(int argc, char *argv[]) {
  int time[ASIZE], delta;
  struct timeval tp1, tp2;
  int i, count0, count1, count2;

  count0 = 0;
  count1 = 0;
  count2 = 0;
  while (count2 < ASIZE){
    gettimeofday(&tp1, NULL);
    gettimeofday(&tp2, NULL);
    delta = (tp2.tv_sec - tp1.tv_sec)*1000000 + (tp2.tv_usec- tp1.tv_usec);
    if      (delta == 0) count0++;
    else if (delta == 1) count1++;
    else if (delta  > 1) time[count2]=delta, count2++;
  }
  printf("Total timings %d \n",count0 + count1 + count2);
  printf("Number of  0 delta time %d \n", count0);
  printf("Number of  1 delta time %d \n", count1);
  printf("Number of >1 delta time %d and values (in usec):\n", count2);
  for (i=0; i<ASIZE; i++) {
     printf("%d ",time[i]);
     if (i%10 == 9)printf("\n");
  }
  printf("\n");
  return 0;
} //main()
