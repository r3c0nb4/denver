#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#define STRIDE 4096
#define ITER 128
int main(){
    uint8_t data = 0;
    uint8_t buffer0[STRIDE];
    uint8_t *ptr0 = &data;
    uint8_t buffer1[STRIDE];
    uint8_t **ptr1 = &ptr0;
    uint8_t buffer2[STRIDE];
    uint8_t ***ptr2 = &ptr1;
    uint8_t buffer3[STRIDE];
    uint8_t ****ptr3 = &ptr2;
    uint8_t buffer4[STRIDE];
    uint8_t *****ptr4 = &ptr3;
    uint8_t buffer5[STRIDE];
    volatile uint8_t pick = 0;

    uint64_t init = 0;
    uint64_t end = 0;
    /*
     * chaining cache miss 
    */
   
   for(int i = 0; i < ITER; i++){
 	    cacheflush(&data);
 	    cacheflush(&ptr0);
 	    cacheflush(&ptr1);
 	    cacheflush(&ptr2);
		cacheflush(&ptr3);
		cacheflush(&ptr4);
		barrier();		
		for(volatile int z = 0; z < 100; z++){
		}
		barrier();
        init = get_cycles();
        pick = *****ptr4;
        end = get_cycles();

        printf("latency cache miss: %ld\n", end - init);
   }


    return 0;

}
