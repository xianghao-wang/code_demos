#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define M 4

static int rank, nprocs;
int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  int N = M * nprocs;
  int *A = malloc(sizeof(int) * M);
  int *B = malloc(sizeof(int) * M);

  // Prepare data
  for (int i = 0; i < M; i ++)
    A[i] = rank * M + i;

  // Shuffle
  int nReq = 0;
  MPI_Request reqs[M*2];
  for (int i = 0; i < M; i ++)
  {
    int idx = rank * M + i;
    int idy = idx < N / 2 ? 2 * idx : 2 * (idx-N/2) + 1;
    int partner = idy / M;

    if (partner != rank)
      MPI_Isend(&A[i], 1, MPI_INT, partner, idy % M, MPI_COMM_WORLD, &reqs[nReq ++]);
    else
      B[idy%M] = A[i];

    idy = idx % 2 ? (idx - 1) / 2 + N / 2 : idx / 2;
    partner = idy / M;
    if (partner != rank)
      MPI_Irecv(&B[i], 1, MPI_INT, partner, i, MPI_COMM_WORLD, &reqs[nReq ++]);
    else
      B[i] = A[idy%M];
  }
  MPI_Waitall(nReq, reqs, MPI_STATUS_IGNORE);

  for (int i = 0; i < nprocs; i ++)
  {
    if (i == rank)
    {
      printf("Process %d:", rank);
      for (int j = 0; j < M; j ++) printf(" %d", B[j]);
      printf("\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }


  free(A);
  free(B);
  MPI_Finalize();
  return 0;
}