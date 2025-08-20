#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include "utils.h"
#define STRIDE 4096
#define ITER 8192 * 2
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

#define REPEAT_10(x) x x x x x x x x x x

static inline __attribute__((always_inline)) uint64_t func(uint64_t *init, uint64_t *end){
	isb();
	*init = get_cycles();
	MOV(100);
	*end = get_cycles();
	isb();
}
	
static inline __attribute__((always_inline)) void measure_time() {
	uint64_t init;
	uint64_t end;
	volatile uint64_t pick;
#ifdef N
	NOPS(N);
#endif
	for(int i = 0; i < 10000; i++){
//		isb();
//        init = get_cycles();		
//		MOV(200);
//        end = get_cycles();
//		isb();
		func(&init, &end);
		printf("%lu\n", end - init);
	}
}

int main(){
	uint8_t score[5] = {0};
	uint8_t index = 0;
    volatile uint64_t pick = 0;
    uint64_t init = 0;
    uint64_t end = 0;
	//REPEAT_10(measure_time();)
	measure_time();
	//for(int i = 0; i < 10; i++){
	//	measure_time();
	//}
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();
//	measure_time();

//	for(int i = 0; i < 1024; i++){
//		asm volatile(
//			"ldr x1, %[val]\n"
//			:
//			:[val] "m" (pick)
//			: "x1"
//		);
//		for(volatile int z = 0; z < 100; z++){}
//		isb();
//        init = get_cycles();		
//		NOPS(100);
//		asm volatile(
//			"adds x1, x1, #1\n"
//			::: "x1"
//		);
//        end = get_cycles();
//		isb();
//		asm volatile(
//			"str x1, %[val]\n"
//			:
//			: [val] "m" (pick)
//			: "x1"
//		);
//        printf("%ld\n", end - init);
//		if(score[4] == 1){
//			break;
//		}
//		if((end - init) - 50 < 5 || (end - init) - 50 > -5){
//			score[index] = 1;
//			index ++;
//		}
//	}
	

}
