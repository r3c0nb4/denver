#ifndef _UTILS_H
#define _UTILS_H
#include <time.h>
#include <stdint.h>
#define BILLION 1000000000L
#define probe_size 4096
#define iter 100
#define random(a, b) (rand()%(b - a + 1) + a)

uint64_t  CACHE_THRESHOLD = 0;

typedef struct _prefetch_result{
  uint64_t result[iter];
}PREFETCH;

static inline __attribute__((always_inline)) void cacheflush(void* address)
{
  asm volatile ("DC CIVAC, %0" :: "r"(address));
  asm volatile ("DSB ISH");
  asm volatile ("ISB");
}

static inline __attribute__((always_inline)) void barrier(void){
  asm volatile ("DMB SY ");
}

static inline __attribute__((always_inline)) void isb(void){
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

static uint64_t timed_read(volatile uint8_t *addr, struct timespec *start, struct timespec *end) {
	volatile uint8_t pick = 0; 
  clock_init(start);
	pick = *addr;
  clock_end(end);
	barrier();
  return get_clock(*start, *end);
}

static inline uint64_t get_cycles(void)
{
	uint64_t val;
	asm volatile("mrs %0, pmccntr_el0" : "=r"(val));
	return val;
}

static inline uint64_t cycles_read(volatile uint8_t *addr){
	volatile uint8_t pick = 0;
	uint64_t cycles1 = 0, cycles2 = 0;
	
	cycles1 = get_cycles();	
	pick = *addr;
	cycles2 = get_cycles();
	barrier();
	return cycles2 - cycles1;

}
void bubble_sort(uint64_t *arr, int len) {
    int i, j, temp;
    for (i = 0; i < len - 1; i++)
		for (j = 0; j < len - 1 - i; j++)
			if (arr[j] > arr[j + 1]) {
 				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
            }
}

#endif
