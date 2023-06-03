#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define M 100
#define N 100
static int nprocs;
static int rank;

int compute_sum(int *nums, int len)
{
  int res[2] = {0, 0}; // sum and next start

  // Receive the result from the previous node
  if (rank > 0) MPI_Recv(res, 2, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // Perform summation
  for (int i = res[1]; i < len+res[1] && i < N; i ++)
    res[0] += nums[i];
  res[1] += len;

  // Send to next node
  if (rank + 1 < nprocs)
    MPI_Send(res, 2, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
  
  return res[0];
}

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  int nums[M][N];
  int sum[M];

  // Give random data
  srand(time(NULL));
  for (int i = 0; i < M; i ++)
    for (int j = 0; j < N; j ++)
      nums[i][j] = rand() % 20;

  // Start pipeline
  int len = (N + nprocs - 1) / nprocs;
  for (int i = 0; i < M; i ++)
    sum[i] = compute_sum(nums[i], len);

  if (rank == nprocs - 1) 
  {
    // Verify
    int sum_seq[M];
    for (int i = 0; i < M; i ++)
    {
      sum_seq[i] = 0;
      for (int j = 0; j < N; j ++)
        sum_seq[i] += nums[i][j];
    }

    for (int i = 0; i < M; i ++)
      if (sum[i] != sum_seq[i])
      {
        printf("Wrong: %d vs %d at %d!\n", sum[i], sum_seq[i], i);
        break;
      }
    printf("Done\n");
  }
  

  MPI_Finalize();
  return 0;
}