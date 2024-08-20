#include "common.h"

#include <time.h>
#include <stdio.h>
#include <unistd.h>

static const double TIMEIT_SECONDS = 10;

#define ANSII_BLUE "\e[0;34m"
#define ANSII_CLEAR "\e[0m"

void timeit(const char *description, void (*func)(void *args), void *args) {
  struct timespec start, end;

  double diff = 0;
  size_t iters = 0;

  printf("%s benchmark should take ~%.2f seconds...\n", description, TIMEIT_SECONDS);

  clock_gettime(CLOCK_MONOTONIC, &start);

  for (iters = 0; diff < TIMEIT_SECONDS && iters < __SIZE_MAX__; ++iters) {
    func(args);

    clock_gettime(CLOCK_MONOTONIC, &end);
    diff = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1.0e9;
  }

  double average_ms = diff / iters * 1000;

  printf("%s took " ANSII_BLUE "%f ms" ANSII_CLEAR " on average (ran %lu iterations in %.2f seconds).\n", 
    description, average_ms, iters, diff);
}