#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

#include "mpi.h"

int main(int argc, char **argv)
{
  int rank, nprocs, *numbers;
  int i, slave, offset, sum, part_sum;
  double t;
  int n = atoi(argv[1]);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int s = n / (nprocs - 1);
  if (rank == 0)
  {
    
    
    // Prepare data
    numbers = (int *) malloc(n * sizeof(int));
    srand(time(NULL));
    for (i = 0; i < n; i ++) numbers[i] = rand() % 10;
  
    t = MPI_Wtime();
    // Distribute task
    offset = 0;
    for (slave = 1; slave < nprocs; slave ++)
    {
      MPI_Send(numbers + offset, s, MPI_INT, slave, 0, MPI_COMM_WORLD);
      offset += s;
    }

    // Calculate total sum
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
    s = n / (nprocs - 1);
    numbers = (int *) malloc(s * sizeof(int));
    
    // Receive data
    MPI_Recv(numbers, s, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // Calculate partial sum
    part_sum = 0;
    for (i = 0; i < s; i ++) part_sum += numbers[i];

    // Send the result to master
    MPI_Send(&part_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  free(numbers);
  MPI_Finalize();
  return 0;
}