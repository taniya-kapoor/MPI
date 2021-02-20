# include <stdio.h>
# include <stdlib.h>

double f(double x){
  return 4./(1+x*x);
}

int main(int argc, char* argv[]){
  int n=24;
  double a=0., b=1.;
  double sum=0.;
  double h=(b-a)/n;

  for (int i=0; i<n; i++){
    sum+=h*f(i*h+h/2);
  }

  printf("Approximation of PI : %.10f\n", sum);
  return 0;
}
