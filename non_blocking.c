# include <mpi.h>
# include <stdlib.h>
# include <stdio.h>

int main(int argc, char* argv[]){
  MPI_Status status;
  int tag=1, rank, proc;
  MPI_Request req_send, req_recv;
  int a, b;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  a=1;
  
  if (rank==0){
    MPI_Isend(&a, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &req_send);
  }
  else if (rank==1){
    MPI_Irecv(&b, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &req_recv);
  }
  for (int i=0; i<100; i++){
    printf("i=%d\n", i);
  }
  if (rank==1)
    {
      MPI_Wait(&req_recv, &status);
    }
  b+=1;
  printf("rank=%d, b=%d\n", rank, b);

  MPI_Finalize();
}

