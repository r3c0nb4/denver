#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/mman.h>
#include "utils.h"
#define ITER 8192 * 2


int main(){
	uint64_t cycles_dependency[ITER];

	srand(time(NULL));
	uint64_t random2 = 0;
	uint64_t random0 = 0;
	uint64_t random1 = 0;
    uint64_t init = 0;
    uint64_t end = 0;
	uint64_t result_add= 0;
	uint64_t result_div = 0;
	
	/*
	 * data dependency latency measurement
	 */

	for(int i = 0; i < ITER; i++){
		random2 = rand() % 11;
		random0 = rand() % 11;
		random1 = rand() % 11;

		asm volatile(
			"ldr x1, %[var0]\n"
			"ldr x2, %[var1]\n"
			"ldr x3, %[var2]\n"
			"mov x4, #1024\n"
			:
			:[var0] "m" (random2), [var1] "m" (random0), [var2] "m" (random1)
			:"x1", "x2", "x3", "x4"			
		);
		for(volatile int z = 0; z < 100; z++){}

		isb();
		init = get_cycles();
		asm volatile(
			".rept 40\n"
			"adds x2, x2, x1\n"
			"adds x3, x3, x2\n"
			"mov x1, x3\n"
			".endr\n"
			"sdiv x4, x4, x1\n"
			::: "x1", "x2", "x3", "x4"
		);
		end = get_cycles();
		isb();

		cycles_dependency[i] = end - init;
		asm volatile(
			"str x1, %[var]\n"
			"str x4, %[var0]\n"
			:
			: [var] "m" (result_add), [var0] "m" (result_div)
			: "x1", "x4"
		);
		
	}

	for(int i = 0; i < ITER; i++){
		printf("%ld\n", cycles_dependency[i]);
	}
	return 0;
}
