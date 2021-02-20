# include <stdio.h>
# include <mpi.h>

int main(int argc, char* argv[]){
  int nbProc, myRank, l;
  char procname[1024];
  int a=0, b, tag, i;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Get_processor_name(procname, &l);

  tag=1;
  b=a;
  if (myRank!=0){
    a=myRank;
    MPI_Send(&a, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
  }
  else {
    for (i=1; i<nbProc; i++){
      MPI_Recv(&a, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
      b+=a;
    }
  }

  if (myRank==0){
    for (i=1; i<nbProc; i++){
      MPI_Send(&b, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
    }
  }
  else {
    MPI_Recv(&b, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
  }
 
  printf("CPU %d: b=%d\n", myRank, b);

  MPI_Finalize();
}
