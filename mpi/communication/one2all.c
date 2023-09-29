#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

static int rank, nprocs;

// Communication partner oriented
void method1(int *x, int len)
{
  int mask = nprocs >> 1; // distance
  while (mask > 0)
  {
    if (!(rank & (mask-1)))
    {
      int partner = rank ^ mask;
      if (rank & mask)
        MPI_Recv(x, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      else
        MPI_Send(x, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
    }

    mask >>= 1;
  }
}

// Communication distance orientated 
void method2(int *x, int len)
{
  for (int d = nprocs / 2; d > 0; d /= 2)
  {
    if (rank % (2 * d) == 0)
      MPI_Send(x, 1, MPI_INT, rank + d, 0, MPI_COMM_WORLD);
    else if (rank % (2 * d) == d)
      MPI_Recv(x, 1, MPI_INT, rank - d, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
  }
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int x = 0;
  if (rank == 0) x = 15;

  // method1(&x, 1);
  method2(&x, 1);
  
  printf("rank %d with value %d\n", rank, x);

  MPI_Finalize();

  return 0;
}