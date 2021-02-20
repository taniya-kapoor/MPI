# include <stdio.h>
# include <mpi.h>

int main(int argc, char* argv[]){
  int nbProc, myRank, l;
  char procname[1024];
  int a, tag=1, i;
  int N=10;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  if (myRank==0){
    int* a=(int*) malloc(sizeof(int)*N);
    for (i=0; i<N; i++){
      a[i]=1;
    }
  };
  
  if (myRank==0){
    for (i=1; i<nbProc; i++){
      MPI_Send(&a, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
    }
  }
  else {
    MPI_Recv(&a, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
  }
  printf("CPU %d: a=%d\n", myRank, a);

  MPI_Finalize();
}
