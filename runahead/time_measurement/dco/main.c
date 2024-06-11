#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include "utils.h"
#define STRIDE 4096
#define ITER 8192 * 2
#ifndef N
#define N 100 // Default value if not defined in Makefile
#endif
#define NOPS(n) \
    asm volatile( \
        ".rept %0\n" \
        "nop\n" \
        ".endr\n" \
        :: "i" (n) \
    )

#define MOV(n)	\
	asm volatile(	\
		".rept %0\n" \
		"mov x10, x10\n" \
		".endr\n"	\
		::"i" (n): "x10" \
	)


int main(){
    volatile uint64_t pick = 0;
    uint64_t init = 0;
    uint64_t end = 0;
	for(int i = 0; i < ITER; i++){
		asm volatile(
			"ldr x1, %[val]\n"
			:
			:[val] "m" (pick)
			: "x1"
		);
		for(volatile int z = 0; z < 100; z++){}
		isb();
        init = get_cycles();		
		MOV(N);
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
}
