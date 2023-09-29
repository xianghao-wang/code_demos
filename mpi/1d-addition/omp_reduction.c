#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000

int main()
{
  int np = omp_get_max_threads();
  int x[N];
  int i, d;
  int expected = 0;

  // Prepare data
  srand(time(NULL));
  for (i = 0; i < N; i ++) 
  {
    x[i] = rand() % 20;
    expected += x[i];
  }

  // Reductive summation
  #pragma omp parallel default(none) private(i, d) shared(np, x)
  {
    d = np;
    int n = N;
    int tid = omp_get_thread_num();
    while (d > 0)
    {
      if (tid < d)
        for (int i = d+tid; i < n; i += d)
          x[tid] += x[i];
      #pragma omp barrier
      n = d;
      d /= 2;
    }
  }

  int sum = x[0];
  printf("Actual: %d\n", sum);
  printf("Expected: %d\n", expected);

  return 0;
}