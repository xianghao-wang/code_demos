#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 4

static int rank, nprocs;
int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  int *x = malloc(sizeof(int) * N);
  int *shuffled = malloc(sizeof(int) * N * 2);

  // Prepare data
  for (int i = 0; i < N; i ++)
    x[i] = rank * N + i;

  // Step 1: shuffle
  MPI_Datatype stride2;
  MPI_Type_vector(N, 1, 2, MPI_INT, &stride2);
  MPI_Type_commit(&stride2);
  int partner = rank >= nprocs / 2 ? rank - nprocs / 2 : rank + nprocs / 2;
  if (rank < partner)
  {
    MPI_Recv(shuffled+1, 1, stride2, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = 0; i < N; i ++) shuffled[2*i] = x[i];
  }
  else
    MPI_Send(x, N, MPI_INT, partner, 0, MPI_COMM_WORLD);

  // Step 2: gather
  if (rank == 0)
  {
    int *all = malloc(sizeof(int) * N * nprocs * 2);
    MPI_Gather(shuffled, 2 * N, MPI_INT, all, 2 * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Print result
    for (int i = 0; i < N * nprocs; i ++)
      printf("%d ", all[i]);
    putchar('\n');

    free(all);
  }
  else
  {
    MPI_Gather(shuffled, 2 * N, MPI_INT, NULL, 2 * N, MPI_INT, 0, MPI_COMM_WORLD);
  }

  free(x);
  free(shuffled);
  MPI_Finalize();
  return 0;
}