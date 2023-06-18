#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>

#define DEBUG 0

/*********************************************************************/
/* This program solves a set of linear equations of the form Ax=b    */
/*********************************************************************/

void iprtmatrix(const char label[], int* matrix, int ncol, int nrow);
void prtmatrix(const char label[], double* matrix, int ncol, int nrow);
void gauss_elim(double a[], double b[], int n, double x[], 
                double sz[], int ord[]);
void largest(double a[],int n, double sz[], int ord[]);
void elim(double a[], double b[], int n, double sz[], int ord[]);
void pivot(double a[], double sz[], int ord[], int n, int k);
void subst(double a[], double b[], int n, double x[], int ord[]);
double cpu_time(double *tcpu, double *tsys);

int main(int argc, char* argv[]){

  double *amatrix, *amatrix_kp; 
  double *bvector, *bvector_kp, *xvector, *szvector, *resvector;
  int *ordvector;
  int nsize;
  time_t t1;
  int i,j;
  double temp, error;
  double ttot0, tcpu0, tsys0;
  double ttot1, tcpu1, tsys1;


  if (argc != 2) {
    printf(" %s Problem Dimension \n",argv[0]);
    return -1;
  }
    else {
      nsize = atoi(argv[1]);
  }

  printf("\n************************************************************\n");
  printf(  "         Program to solve a set of linear equations         \n");
  printf(  "************************************************************\n");
  printf("\n Problem size = %d \n",nsize);

/* Allocate Memory */
  amatrix    = (double*) malloc(nsize*nsize*sizeof(double));
  amatrix_kp = (double*) malloc(nsize*nsize*sizeof(double));
  bvector    = (double*) malloc(nsize*sizeof(double));
  bvector_kp = (double*) malloc(nsize*sizeof(double));
  xvector    = (double*) malloc(nsize*sizeof(double));
  szvector   = (double*) malloc(nsize*sizeof(double));
  ordvector  = (int*)    malloc(nsize*sizeof(int));

  if (!( amatrix && amatrix_kp && bvector && bvector_kp &&
         xvector && szvector && ordvector) ){
      printf(" Allocation failure reduce n %d \n",nsize);
      exit(-1);
  } 

/*Initialize problem to some random data */
  (void) time(&t1);  /* use time to seed random number */
  srand48((long) t1);
  for (i=0; i<nsize*nsize; i++)amatrix_kp[i] = drand48();
  for (i=0; i<nsize; i++)      bvector_kp[i] = drand48();

  if (DEBUG){    
    prtmatrix("Initial Amatrix ",amatrix_kp, nsize, nsize);
    prtmatrix("Initial bvector ",bvector_kp, 1, nsize);
  }

  for (i=0; i<nsize*nsize; i++)amatrix[i] = amatrix_kp[i];
  for (i=0; i<nsize; i++)      bvector[i] = bvector_kp[i];
  for (i=0; i<nsize; i++)      xvector[i] = 0.0;

  ttot0=cpu_time(&tcpu0, &tsys0);
  gauss_elim(amatrix, bvector, nsize, xvector, szvector, ordvector);
  ttot1=cpu_time(&tcpu1, &tsys1);

  if (DEBUG){    
    prtmatrix("Solution vector ",xvector, 1, nsize);
  }


/*Evaluate error in solution*/
  error=0.0;
  for (i=0; i< nsize; i++){
    temp=bvector_kp[i];
    for (j=0; j< nsize; j++){
      temp-=amatrix_kp[i*nsize+j]*xvector[j];
    }
    error += temp*temp;
  }

  error = sqrt(error);
  printf(" RMS error in solution = %20.8e\n",error);
  printf(" (Error should be small ~machine precision)\n\n");

  printf("     CPU time = %10.2f\n",tcpu1-tcpu0);
  printf("     SYS time = %10.2f\n",tsys1-tsys0);
  printf(" CPU+SYS time = %10.2f\n",ttot1-ttot0);

  return 0;
}

void gauss_elim(double a[], double b[], int n, double x[], 
                double sz[], int ord[]){

  largest(a,n,sz,ord);
  elim(a,b,n,sz,ord);
  subst(a,b,n,x,ord);
  return;
}

void largest(double a[], int n, double sz[], int ord[]){
  int i,j;

  for (i=0; i<n; i++){
     ord[i]=i;
     sz[i]=fabs(a[i*n]);
     for (j=1; j<n; j++)if (fabs(a[n*i+j]) > sz[i]) sz[i] = fabs(a[n*i+j]);
  }
  return;
}

void elim(double a[], double b[], int n, double sz[], int ord[]){
  int i,j,k;
  double abs, factor;
  double *temp;

  for (k=0; k<n-1; k++){
    pivot(a,sz,ord,n,k);
    for (i=k+1; i<n; i++){
      factor=a[ord[i]*n+k]/a[ord[k]*n+k];
      for (j=k+1; j<n; j++){
          a[ord[i]*n+j]=a[ord[i]*n+j]-factor*a[ord[k]*n+j];
      }
      b[ord[i]]=b[ord[i]]-factor*b[ord[k]];
    }
  }

  temp = (double*) malloc(n*n*sizeof(double));
  for (i=0; i<n*n; i++)temp[i]=0.0;
  for (i=0; i<n; i++){
    sz[i]=b[ord[i]];
    for (j=i; j<n; j++){
      temp[i*n+j]=a[ord[i]*n+j];
    }
  }
  for (i=0; i<n*n; i++)a[i]=temp[i];
  for (i=0; i<n;   i++)b[i]=sz[i];
  free(temp);
}

void pivot(double a[], double sz[], int ord[], int n, int k){
  int idxpvt,ii,idum;
  double big,test;

  idxpvt=k;
  big=fabs(a[ord[k]*n+k]/sz[ord[k]]);

  for (ii=k+1; ii<n; ii++){
     test=fabs(a[ord[ii]*n+k]/sz[ord[ii]]);
     if (test > big){
        big=test;
        idxpvt=ii;
     }
  }

  idum=ord[idxpvt];
  ord[idxpvt]=ord[k];
  ord[k]=idum;

}

void subst(double a[], double b[], int n, double x[], int ord[]){
  int i,j;
  double sum;

  x[n]=b[n]/a[n*n-1];
  for (i=n-1; i>=0; i--){
    sum=0.0;
    for (j=i+1; j<n; j++){
      sum=sum+a[i*n+j]*x[j];
    }
    x[i]=(b[i]-sum)/a[i*n+i];
  }
  return;
}

void prtmatrix(const char label[], double* matrix, int ncol, int nrow){
  int icount,row,col;

  printf("\n Prtmatrix : %-40s \n",label);
  icount=0;
  for (row=0; row<nrow; row++){
    printf("Row%4d ",row);
    for (col=0; col<ncol; col++){
       printf("%12.3e",matrix[icount]);
       icount++;
       if (col%6 == 5)printf("\n        ");
    }
    printf("\n");
  }
}
void iprtmatrix(const char label[], int* matrix, int ncol, int nrow){
  int icount,row,col;

  printf("\n iPrtmatrix : %-40s \n",label);
  icount=0;
  for (row=0; row<nrow; row++){
    printf("Row%4d ",row);
    for (col=0; col<ncol; col++){
       printf("%12d",matrix[icount]);
       icount++;
       if (col%6 == 5)printf("\n        ");
    }
    printf("\n");
  }
}

double cpu_time(double *tcpu, double *tsys)
{
    struct tms timestruct;
    long sec0, sec1;

    static long tick = 0;
    if(!tick)tick = sysconf(_SC_CLK_TCK);

    times(&timestruct);
    
    sec0 = timestruct.tms_utime/tick;
    sec1 = timestruct.tms_utime - sec0*tick;
    *tcpu= (double) sec0 + (double) sec1 / (double) tick;

    sec0 = timestruct.tms_stime/tick;
    sec1 = timestruct.tms_stime - sec0*tick;
    *tsys= (double) sec0 + (double) sec1 / (double) tick;

    return *tcpu + *tsys;
}
