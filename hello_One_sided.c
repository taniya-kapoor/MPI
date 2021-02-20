# include <stdio.h>
# include <mpi.h>

int main(int argc, char* argv[]){
  int nbProc, myRank, l;
  char procname[1024];
  int a, tag1, tag2;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Get_processor_name(procname, &l);

  tag1=1;
  if (myRank==0) a=1;
  if (myRank==0) MPI_Send(&a, 1, MPI_INT, 1, tag1, MPI_COMM_WORLD);
  if (myRank==1) MPI_Recv(&a, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status);
  printf("CPU %d: a=%d\n", myRank, a);

  MPI_Finalize();
}
