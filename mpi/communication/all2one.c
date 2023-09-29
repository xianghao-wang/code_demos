#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int rank, nprocs;

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (nprocs & (nprocs - 1))
  {
    printf("Number of processes need to be power of 2!\n");
    exit(1);
  }

  srand(rank);
  int *x = malloc(sizeof(int) * nprocs);
  x[rank] = rand() % 20;

  int mask = 0x1;
  while (mask < nprocs)
  {
    int partner = rank ^ mask;
    int myIdx = rank & ~(mask - 1);
    int pIdx = partner & ~(mask - 1);

    if (rank < partner)
      MPI_Recv(x+pIdx, mask, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    else
      MPI_Send(x+myIdx, mask, MPI_INT, partner, 0, MPI_COMM_WORLD);

    mask <<= 1;
  }

  if (rank == 0)
  {
    for (int i = 0; i < nprocs; i ++)
      printf("%d ", x[i]);
    printf("\n");
  }

  MPI_Finalize();

  return 0;
}