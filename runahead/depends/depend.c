#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "utils.h"

#define STRIDE 4096
#define RELOAD_BUF_SIZE STRIDE * 256
#define ITER 1024

uint8_t *reloadbuffer;
volatile uint8_t pick;
int main(){
	int result[256];
	int index;
	register uint64_t measured_clock;
	register unsigned char c = 0;
	struct timespec start, end;

	reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	unsigned char buffer[4096];
	char *secret = "123456";
	

	for(int j = 0; j < ITER; j++){
		cacheflush(&secret);
		for (int i = 0; i < 256; i++){
			cacheflush(&reloadbuffer[i * STRIDE]);
		}
		barrier();
	
		for (volatile int z = 0; z < 100; z++){
		}
		barrier();
	
		// data dependency
		//counter = 1;
		//counter = cache;
		c = secret[2];
		pick = reloadbuffer[c << 12];
		
		for (int i = 0; i < 256; i++)
		{
			index = ((i * 167) + 13) & 255;
			measured_clock = timed_read(&reloadbuffer[index * STRIDE], &start, &end);
			if (measured_clock <= 100){
				result[index]++; 
			}
		}	
	}

	for(int i = 0; i < 256; i++){
		if(result[i] != 0){
			printf("%c\n", i);
		}
	}
}
