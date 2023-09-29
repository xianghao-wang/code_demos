#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

#include "mpi.h"

int main(int argc, char **argv)
{
  int rank, nprocs, *numbers;
  int i, s, slave, offset, sum, part_sum;
  double t;
  int n = atoi(argv[1]);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  numbers = (int *) malloc(n * sizeof(int));
  // Prepare data
  if (rank == 0)
  {
    srand(time(NULL));
    for (i = 0; i < n; i ++) numbers[i] = rand() % 10;
  }
  
  t = MPI_Wtime();
  // Broadcast data  
  MPI_Bcast(numbers, n, MPI_INT, 0, MPI_COMM_WORLD);
  s = n / (nprocs - 1);

  if (rank == 0)
  {
    // Calculate sum
    sum = 0;
    for (slave = 1; slave < nprocs; slave ++)
    {
      MPI_Recv(&part_sum, 1, MPI_INT, slave, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      sum += part_sum;
    }
    printf("Execution time: %.3es\n", MPI_Wtime() - t);

    // Verify
    int expected = 0;
    for (i = 0; i < n; i ++) expected += numbers[i];
    printf("Total sum: %d v.s. %d\n", sum, expected);
  }
  else
  {
    int start, end;
    part_sum = 0;
    start = (rank - 1) * s;
    end = start + s;
    for (i = start; i < end; i ++) part_sum += numbers[i];
    
    // Send result to master
    MPI_Send(&part_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  free(numbers);
  MPI_Finalize();
  return 0;
}