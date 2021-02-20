# include <stdlib.h>
# include <stdio.h>
# include <mpi.h>

int main(int argc, char* argv[]){
  MPI_Status status;
  int tag=1;
  int commsize, rank;

  int a=1;
  double b=2;
  char c[100];
  int position=0; // DO NOT FORGET TO INITIALIZE TO 0 !
  char buffer[1000]; // see it as a memory block that may not necesseraly store characters

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if (rank==0){
    MPI_Pack(&a, 1, MPI_INT, buffer, 1000, &position, MPI_COMM_WORLD);
    MPI_Pack(&b, 1, MPI_DOUBLE, buffer, 1000, &position, MPI_COMM_WORLD);
    MPI_Pack(c, 100, MPI_CHAR, buffer, 1000, &position, MPI_COMM_WORLD);
    MPI_Send(buffer, 1000, MPI_PACKED, 1, tag, MPI_COMM_WORLD);
  }
  else if (rank==1){
    MPI_Recv(buffer, 1000, MPI_PACKED, 0, tag, MPI_COMM_WORLD, &status);
    // be sure that the variable position is initialized to 0
    MPI_Unpack(buffer, 1000, &position, &a, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Unpack(buffer, 1000, &position, &b, 1, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Unpack(buffer, 1000, &position, c, 100, MPI_CHAR, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
