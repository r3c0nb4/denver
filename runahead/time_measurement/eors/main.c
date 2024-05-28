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
        asm volatile(
    	    "ldr x10, %[data]\n"
		    "mov x11, 2\n"
            :: [data] "m" (counter)
			:"x10", "x11"
        );
		for(volatile int z = 0; z < 100; z++){
        }
        init = get_cycles();
        asm volatile(
			".rept 1000\n"
			"eor x10, x10, x11\n"
			".endr\n"
			:::"x10", "x11"
		);
        end = get_cycles();

        printf("latency 1000 eor: %ld\n", end - init);
   }


    return 0;

}
