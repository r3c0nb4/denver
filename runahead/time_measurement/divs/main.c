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
        );
		for(volatile int z = 0; z < 100; z++){
        }
        init = get_cycles();
        asm volatile(
			".rept 10\n"
			"sdiv x10, x10, x11\n"
			".endr\n"
		);
        end = get_cycles();

        printf("latency 100 sdivs: %ld\n", end - init);
   }


    return 0;

}
