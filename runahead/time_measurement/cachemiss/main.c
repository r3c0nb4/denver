#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include "utils.h"
#define STRIDE 4096
#define ITER 32

#define NOPS(n) \
    asm volatile( \
        ".rept %0\n" \
        "nop\n" \
        ".endr\n" \
        :: "i" (n) \
    )

static uint8_t *datas0;

void func (uint8_t *data0, uint8_t *data1){
		volatile uint8_t pick;
		uint64_t init = 0;
		uint64_t end = 0;

 	    cacheflush(data0);
		cacheflush(data1);
		barrier();		
		for(volatile int z = 0; z < 100; z++){
		}
		barrier();
        init = get_cycles();
		pick = *data0;
		
	//	pick = data1;
		NOPS(300);
		pick = *data1;

        end = get_cycles();
		barrier();

        printf("latency cache miss: %ld\n", end - init);
}

int main(){
    uint8_t data0 __attribute__((aligned(4096))) = 8;
    uint8_t data1 __attribute__((aligned(4096))) = 200;
    uint8_t data2 __attribute__((aligned(4096))) = 0;
    uint8_t data3 __attribute__((aligned(4096))) = 0;
    uint8_t data4 __attribute__((aligned(4096))) = 0;
    uint8_t data5 __attribute__((aligned(4096))) = 0;
    volatile uint8_t pick = 0;
	datas0 = (uint8_t *)mmap(0, STRIDE * 256, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);

    uint64_t init = 0;
    uint64_t end = 0;
    /*
     * chaining cache miss 
    */
	for(int i = 0; i < ITER; i++){
		func(&data0, &data1);
// 	    cacheflush(&data0);
//		cacheflush(&data1);
//		barrier();		
//		for(volatile int z = 0; z < 100; z++){
//		}
//		barrier();
//        init = get_cycles();
//		pick = data0;
//		
//		NOPS(300);
//		pick = data1;
//
//        end = get_cycles();
//		barrier();
//
//        printf("latency cache miss: %ld\n", end - init);
	}
		
		isb();

	
 	    cacheflush(&data0);
		cacheflush(&data1);
		cacheflush(&data2);
		cacheflush(&data3);
		cacheflush(&data4);
		cacheflush(&data5);
		barrier();		
		for(volatile int z = 0; z < 100; z++){
		}
		barrier();
        init = get_cycles();
		pick = data0;
		
		pick = data1;
		NOPS(500);
		//pick = data1;
//		NOPS(60);
//		barrier();
//		pick = data2;
//		pick = data3;
//		NOPS(10);
//		barrier();
		
//		pick = data4;
//		pick = data5;
        end = get_cycles();
		barrier();

        printf("latency cache miss single: %ld\n", end - init);
    return 0;

}
