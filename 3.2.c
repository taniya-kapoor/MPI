# include <stdlib.h>
# include <stdio.h>
# include <mpi.h>

int** allocation_2d_int(int n, int m){
  int** a;
  int i;
  
  a=malloc(sizeof(int*)*n);
  a[0]=malloc(sizeof(int)*n*m);
  for (i=1; i<n; i++)
    a[i]=a[i-1]+m;

  return a;
}

int main(int argc, char* argv[]){
  int commsize, rank; // "communicator size"
  int N; // nb of lines of global matrix
  int M=10; // nb of columns of global matrix and local matrix
  int Nloc; // nb of lines of local matrix
  int **a, **aloc;
  MPI_Status status;
  int tag=1;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commsize);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  N=commsize;
  Nloc=N/commsize;

  // allocation of a for CPU 0
  if (rank==0){
    a=allocation_2d_int(N,M);
    for (int i=0; i<N; i++){
      for (int j=0; j<M; j++){
	a[i][j]=1;
      }
    }
  }
  
  aloc=allocation_2d_int(Nloc,M); // allocation of aloc for all CPUs
  
  if (rank==0){
    // extract the part that will be managed by CPU 0
    for (int n=0; n<Nloc; n++){
      for (int m=0; m<M; m++){
	aloc[n][m]=a[n][m];
      }
    }

    for (int R=1; R<commsize; R++){
      MPI_Send(&(a[R*Nloc][0]), Nloc*M, MPI_INT, R, tag, MPI_COMM_WORLD); // CPU 0 send aloc to all CPUs except itself
       }
  }
  else {
    MPI_Recv(&(aloc[0][0]), Nloc*M, MPI_INT, 0, tag, MPI_COMM_WORLD, &status); // all the CPUs receive their aloc
  }

  // we free a in CPU 0
  if (rank==0){
    free(a[0]);
    free(a);
  }
  
  int bloc=0;
  for (int i=0; i<Nloc*M; i++){ // we compute the sum of elements of aloc for all CPUs
    bloc+=aloc[0][i]; 
  }

  int sum=bloc; 
  if (rank!=0){
    MPI_Send(&bloc, 1, MPI_INT, 0, tag, MPI_COMM_WORLD); // All CPUs (except CPU 0) send their sub-sums to CPU 0
  }
  else {
    for (int i=1; i<commsize; i++){ // we compute the sum of elements of a 
      MPI_Recv(&bloc, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status); // CPU 0 receive all the sub-sums
      sum+=bloc;
    }

    printf("The sum of the elements of a is : %d\n", sum);
  }

  free(aloc); // we free aloc in all CPUs
  
  MPI_Finalize();
  return 0;
}
