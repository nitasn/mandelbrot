#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define PANIC(msg) do {                                                         \
  fprintf(stderr, "Runtime error in \"%s:%d\". %s\n", __FILE__, __LINE__, msg); \
  exit(EXIT_FAILURE);                                                           \
} while (0)

#define START_TIMER(name) const double __started_ ## name = omp_get_wtime()

#define PRINT_TIMER(name) printf(#name " took %f seconds\n", (omp_get_wtime() - __started_ ## name))

void timeit(const char *description, void (*func)(void *args), void *args);

#endif // COMMON_H
