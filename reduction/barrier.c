#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
  Self-implemented MPI_Barrier();
*/

static int rank;
static int nprocs;

void my_barrier()
{
  int t[1000000];
  int mask = 0x1;
  int done = 0;
  while (!done && mask < nprocs)
  {
    int partner = rank ^ mask;
    if (rank < partner)
    {
      if (partner < nprocs)
      {
        MPI_Recv(&t, 1000000, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    else
    {
      MPI_Send(&t, 1000000, MPI_INT, partner, 0, MPI_COMM_WORLD);
      done = 1;
    }

    mask <<= 1;
  }
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  // Test barrier
  for (int i = 0; i < nprocs; i ++)
  {
    if (i == rank) printf("I am process %d\n", rank);
    my_barrier();
  }

  MPI_Finalize();
  return 0;
}