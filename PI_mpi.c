# include <stdio.h>
# include <stdlib.h>
# include <mpi.h>

double f(double x){
  return 4./(1+x*x);
}

int main(int argc, char* argv[]){
  int n=1<<24;
  double a=0., b=1.;
  double sum=0.;
  double sum_loc;
  double h=(b-a)/n;
  int nloc;
  double t1, t2, t3;
  
  MPI_Status status;
  int tag=1, rank, nbProc;

  // R=0, i_R=0, ..., nloc-1
  // R=1, i_R=nloc, ..., 2*nloc-1
  // ...
  // R=r, i_R=N*nloc, ..., (r+1)*nloc-1

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(n%nbProc !=0){
    printf("Le nombre de processeur doit être diviseur de 24\n");
    exit(EXIT_FAILURE);
  }

  sum_loc=0.;
  nloc=n/nbProc;

  t1=MPI_Wtime();
  for (int i=rank*nloc; i<=(rank+1)*nloc-1; i++){
    sum_loc+=h*f(i*h+h/2);
  }
  t2=MPI_Wtime();
  MPI_Allreduce(&sum_loc, &sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  t3=MPI_Wtime();
  double Tparallel_comp=t2-t1;
  double Tparallel_comm=t3-t2;
  double Tparallel;
  
  // printf("Approximation of PI : %.10f\n", sum);
  printf("rank= %d, time cost= %.8f\n", rank, t2-t1);

  if (rank==0){
    FILE *file;
    double Tserial, S, S_comm, S_comp;
    if (nbProc==1){
      Tserial=Tparallel_comm+Tparallel_comp;
    }
    else{
      t1=MPI_Wtime();
      sum=0.;
      for (int i=0; i<n; i++){
	sum+=h*f(i*h+h/2);
      }
      t2=MPI_Wtime();
      Tserial=t2-t1;
    }
    S=Tserial/Tparallel;
    S_comm=Tserial/Tparallel_comm;
    S_comp=Tserial/Tparallel_comp;
    file=fopen("speedup_pi.dat","a");
    fprintf(file, "%d \t %.8f \t %.8f \t %.8f \t %.8f \t %.8f \t %.8f\n", nbProc, Tparallel_comm, Tparallel_comp, Tparallel, S_comm, S_comp, S);
    fclose(file);
  }
  
  MPI_Finalize();
  return 0;
}

// Validation of a parallel code :
// 1. It produces the same result independently of the number of processes. (Here, the returned value of pi must depend on n only)
// 2. Evaluate the parallelization performances :
//          N = nbProc
//          T_N = time cost for an execution on N processes
//          S_N = T_S/T_N = time cost of serial rum / tiime cost of parallel run with N CPUs
//    If N |---> S_N is increasing, your code speeds up !
//    It is possible that the curve decrease after a while and it means that we have reach the physical limit of the computer
//
//     ^                            
//     |                           + +  
//     |                        +     +     + +
//     |                     +          + +     + 
//     |                  +   |                   +
//     |               +
//     |            +         |        
//     |          +
//     |         +            |
//     |        +
//     |     +                |
//     |                                                          
//   -------------------------|------------------------------- >
//     |                     N_*       
//
//    We note N_* = number of physical cores on the used computer
//
// 3. Separate the time cost of :
//      - communications on one hand,
//      - computations without communications on the other hand
