#ifndef _UTILS_H
#define _UTILS_H
#include <time.h>
#include <stdint.h>
#define BILLION 1000000000L
#define RELOAD_BUF_SIZE 4906 * 4096
#define STRIDE 8192
#define probe_size 4096
#define iter 100

typedef struct _prefetch_result{
  uint64_t result[iter];
}PREFETCH;

static inline __attribute__((always_inline)) void flush(void* address)
{
  asm volatile ("DC CIVAC, %0" :: "r"(address));
  asm volatile ("DSB ISH");
  asm volatile ("ISB");
}

void clock_init(struct timespec *start){
  clock_gettime(CLOCK_REALTIME, start);
}

void clock_end(struct timespec *end){
  clock_gettime(CLOCK_REALTIME, end);
}

uint64_t get_clock(struct timespec start, struct timespec end){
  return BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
}


int cmp_uint64 (const void * a, const void * b)
{
   return ( *(uint64_t*)a - *(uint64_t*)b );
}

#endif