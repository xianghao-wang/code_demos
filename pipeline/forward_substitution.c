#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

static int nprocs, rank;

#define N 5

/*
  Solve Ax=b
*/
int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // A
  double A[5][5] = {
        {1.0, 0.0, 0.0, 0.0, 0.0},
        {0.6, 1.0, 0.0, 0.0, 0.0},
        {0.3, 0.2, 1.0, 0.0, 0.0},
        {0.4, 0.8, 0.7, 1.0, 0.0},
        {0.9, 0.1, 0.5, 0.6, 1.0}
    };

  // b
  double b[5] = {1.2, -0.4, 2.1, 0.8, -1.5};
  
  // x
  double x[5];

  double sum = b[rank];

  // Receive x0,x1,x2,...,x(rank-1)
  for (int j = 0; j < rank; j ++)
  {
    MPI_Recv(&x[j], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // Forward xj to next node
    if (rank + 1 < nprocs) MPI_Send(&x[j], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
    
    sum -= A[rank][j] * x[j];
  }

  // Send x[rank] to next node
  x[rank] = sum;
  if (rank + 1 < nprocs) MPI_Send(&x[rank], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);

  // Collect the print results
  MPI_Gather(&x[rank], 1, MPI_DOUBLE, x, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);  
  if (rank == 0)
    for (int i = 0; i < nprocs; i ++)
      printf("x%d=%lf\n", i, x[i]);

  MPI_Finalize();
  return 0;
}
