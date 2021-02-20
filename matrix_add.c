# include <stdio.h>
# include <mpi.h>
# include <stdlib.h>
# include <unistd.h>

int** allocation_2d_int(int n, int m){
  int** a;
  
  a=malloc(sizeof(int*)*n);
  a[0]=malloc(sizeof(int)*n*m);
  for (int i=1; i<n; i++)
    a[i]=a[i-1]+m;

  return a;
}

void free_2d_int(int **a){
  free(a[0]);
  free(a);
}

int main(int argc, char* argv[]){
  int nbProc, myRank;
  int tag1=1, tag2=2;
  int **A, **B, **C;
  int *Aloc, *Bloc, *Cloc;
  int Nloc; // nb of lines of local matrix
  int k1=4, k2=4;
  int N=1<<k1; // nb of lines of global matrix
  int M=1<<k2; // nb of columns of global matrix and local matrix
  MPI_Status status1, status2;
  double t1, t2, time_cost, time_cost_min, time_cost_max, time_cost_average;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProc);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  Nloc=N/nbProc;

  t1=MPI_Wtime();
  // allocation of A and B for CPU 0
  if (myRank==0){
    A=allocation_2d_int(N,M);
    B=allocation_2d_int(N,M);
    C=allocation_2d_int(N,M);
    for (int i=0; i<N; i++){
      for (int j=0; j<M; j++){
	A[i][j]=1;
	B[i][j]=2;
      }
    }
  }
  else {
    A=allocation_2d_int(1,1);
    B=allocation_2d_int(1,1);
    C=allocation_2d_int(1,1);
  }
  
  /*  Aloc=allocation_2d_int(Nloc,M); // allocation of Aloc for all CPUs
      Bloc=allocation_2d_int(Nloc,M); // allocation of Bloc for all CPUs*/

  if ((N*M)%nbProc!=0){ // If the number of rows is not divided by nbProc ...
    Nloc=N*M/nbProc+1;
    int *Nloc_tab, *i0_tab;
    Nloc_tab=malloc(sizeof(int)*nbProc);
    i0_tab=malloc(sizeof(int)*nbProc);
    
    Nloc_tab[nbProc-1]=N*M;
    for (int R=0; R<nbProc-1; R++){
      Nloc_tab[R]=Nloc;
      Nloc_tab[nbProc-1]-=Nloc_tab[R];
    }
    
    i0_tab[0]=0;
    for (int R=1; R<nbProc; R++){
      i0_tab[R]=i0_tab[R-1]+Nloc_tab[R-1];
    }
    
    Aloc=malloc(sizeof(int)*Nloc_tab[myRank]);
    Bloc=malloc(sizeof(int)*Nloc_tab[myRank]);
    Cloc=malloc(sizeof(int)*Nloc_tab[myRank]);
      
    MPI_Scatterv(&(A[0][0]), Nloc_tab, i0_tab, MPI_INT, &(Aloc[0]), Nloc_tab[myRank], MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatterv(&(B[0][0]), Nloc_tab, i0_tab, MPI_INT, &(Bloc[0]), Nloc_tab[myRank], MPI_INT, 0, MPI_COMM_WORLD);
    
    for (int i=0; i<Nloc_tab[myRank]; i++){ // we add Aloc to Bloc for all CPUs
      Cloc[i]=Aloc[i]+Bloc[i]; 
    }
    
    MPI_Gatherv(&(Cloc[0]), Nloc_tab[myRank], MPI_INT, &(C[0][0]), Nloc_tab, i0_tab, MPI_INT, 0, MPI_COMM_WORLD);
  }
  else {
    Aloc=malloc(sizeof(int)*Nloc*M);
    Bloc=malloc(sizeof(int)*Nloc*M);
    Cloc=malloc(sizeof(int)*Nloc*M);

    /*if (myRank==0){
    // extract the part that will be managed by CPU 0
    for (int n=0; n<Nloc; n++){
    for (int m=0; m<M; m++){
    Aloc[n][m]=A[n][m];
    Bloc[n][m]=B[n][m];
    }
    }

    for (int R=1; R<nbProc; R++){
    MPI_Send(&(A[R*Nloc][0]), Nloc*M, MPI_INT, R, tag1, MPI_COMM_WORLD); // CPU 0 send Aloc to all CPUs except itself
    MPI_Send(&(B[R*Nloc][0]), Nloc*M, MPI_INT, R, tag2, MPI_COMM_WORLD); // CPU 0 send Bloc to all CPUs except itself
    }

    free_2d_int(A);
    // B will receive the resulting matrix
    }
    else {
    MPI_Recv(&(Aloc[0][0]), Nloc*M, MPI_INT, 0, tag1, MPI_COMM_WORLD, &status1); // all the CPUs receive their Aloc
    MPI_Recv(&(Bloc[0][0]), Nloc*M, MPI_INT, 0, tag2, MPI_COMM_WORLD, &status2); // all the CPUs receive their Bloc
    }*/

    MPI_Scatter(&(A[0][0]), Nloc*M, MPI_INT, &(Aloc[0]), Nloc*M, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&(B[0][0]), Nloc*M, MPI_INT, &(Bloc[0]), Nloc*M, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i=0; i<Nloc*M; i++){ // we add Aloc to Bloc for all CPUs
      Cloc[i]=Aloc[i]+Bloc[i]; 
    }

    MPI_Gather(&(Cloc[0]), Nloc*M, MPI_INT, &(C[0][0]), Nloc*M, MPI_INT, 0, MPI_COMM_WORLD);
  }
  
  t2=MPI_Wtime();
 
  /*if (myRank!=0){
    MPI_Send(&(Bloc[0][0]), Nloc*M, MPI_INT, 0, tag1, MPI_COMM_WORLD); // All CPUs (except CPU 0) send their rows to CPU 0
    }
    else {
    for (int i=1; i<nbProc; i++){ // we compute the sum of elements of a 
    MPI_Recv(&(B[i*Nloc][0]), Nloc*M, MPI_INT, i, tag1, MPI_COMM_WORLD, &status1); // CPU 0 receive all the sub-sums
    for (int j=0; j<Nloc; j++){
    B[j]=&(Bloc[0][0]);
    }
    }
    }*/

  time_cost=t2-t1;
  time_cost_min=time_cost;
  time_cost_max=0;
  time_cost_average=time_cost;

  MPI_Reduce(&time_cost, &time_cost_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
  MPI_Reduce(&time_cost, &time_cost_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce(&time_cost, &time_cost_average, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myRank==0){
    time_cost_average/=nbProc;
    
    FILE *file;
    file=fopen("speedup_matrix_add.dat","a");
    fprintf(file, "%d \t %.8f \t %.8f \t %.8f\n", nbProc, time_cost_min, time_cost_max, time_cost_average);
    fclose(file);
  }

  if (myRank==0){
    for (int i=0; i<N; i++){
      for (int j=0; j<M; j++){
	printf("%d\t", C[i][j]);
	if (j==M-1) printf("\n");
      }
    }
  }

  free(Aloc); // we free Aloc in all CPUs
  free(Bloc); // we free Aloc in all CPUs
  free(Cloc);

  MPI_Finalize();
}
