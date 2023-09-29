#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define M 10
#define N 15

/*
  Process 1's left and right columns replace the Process 2's ones.
*/
int main(int argc, char **argv)
{
  int nprocs, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  // Initialize data
  int data[M][N];
  srand(time(NULL));
  for (int i = 0; i < M; i ++)
    for (int j = 0; j < N; j ++)
      data[i][j] = rank == 0 ? rand() % 20 : 0;

  // Send left by method 1
  MPI_Datatype sVec;
  MPI_Type_vector(1, 1, N, MPI_INT, &sVec);
  MPI_Type_commit(&sVec);
  if (rank == 0)
  {
    MPI_Send(*data, M, sVec, 1, 0, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Recv(*data, M, sVec, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // Send right by method 2
  MPI_Datatype matCol;
  MPI_Type_vector(M, 1, N, MPI_INT, &matCol);
  MPI_Type_commit(&matCol);
  if (rank == 0)
  {
    MPI_Send(*data+N-1, 1, matCol, 1, 0, MPI_COMM_WORLD);
  }
  else
  {
    MPI_Recv(*data+N-1, 1, matCol, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  // Print process 1's two columns
  if (rank == 0)
  {
    printf("Process 1\n");
    for (int i = 0; i < M; i ++)
    {
      printf("%d %d\n", data[i][0], data[i][N-1]);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  
  // Print process 2's two columns
  if (rank == 1)
  {
    printf("Process 2\n");
    for (int i = 0; i < M; i ++)
    {
      printf("%d %d\n", data[i][0], data[i][N-1]);
    }
  } 

  MPI_Finalize();
  return 0;
}