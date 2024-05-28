#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#define STRIDE 4096
#define ITER 128
int main(){


    uint64_t init = 0;
    uint64_t end = 0;
    uint64_t counter = 4096;
    /*
     * chaining cache miss 
    */
   
	for(int i = 0; i < ITER; i++){
        init = get_cycles();
        asm volatile(
			".rept 1000\n"
			"nop\n"
			".endr\n"
		);
        end = get_cycles();

        printf("latency 1000 nops: %ld\n", end - init);

	}
        init = get_cycles();
        asm volatile(
			".rept 1000\n"
			"nop\n"
			".endr\n"
		);
        end = get_cycles();

        printf("latency 1000 nops single: %ld\n", end - init);
	
    return 0;

}
