# include <stdio.h>
# include <mpi.h>

int main(int argc, char* argv[]){
  int nbProc, myRank, l;
  char procname[1024];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
  MPI_Get_processor_name(procname, &l);

  printf("Hello World from %d / %d [%s]\n", myRank, nbProc, procname);

  MPI_Finalize();
}
