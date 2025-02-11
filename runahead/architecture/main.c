#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"
#define STRIDE 4096

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
#define STRIDE 4096
#define RELOAD_BUF_SIZE STRIDE * 256

static unsigned char *reloadbuffer;
volatile unsigned char pick = 0;
uint64_t latency;
int idx;
int hit[256] = {0};
int max = 0;

void handler(int sig){
	printf("handler\n");
	for(int i = 0; i < 256; i++){
		idx = i;	
		latency = cycles_read(&reloadbuffer[idx * STRIDE]);
		if(latency < 160){
			hit[idx]++;
			printf("%d\n", idx);
		}
	}

	exit(0);
}

int main(){

	struct sigaction sa;
    memset (&sa, '\0', sizeof(sa));
    sa.sa_sigaction = (void (*)(int, siginfo_t *, void *))&handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);

	reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	static uint8_t data0 __attribute__((aligned(4096))) = 0;
	static uint8_t *data0_ptr = &data0;
	register uint8_t cycle = 0;
	register uint64_t cnt;

	memset(hit, 0, sizeof(int) * 256);
	cacheflush(&data0);
	for(int j = 0; j < 256; j++){
		cacheflush(&reloadbuffer[STRIDE * j]);
	}		

	for(volatile int z = 0; z < 100; z++){}
	barrier();
	//pick = data0;
	
	NOPS(10);
	
	asm volatile("mrs %0, cntvct_el0" : "=r" (cnt));
	//*((volatile char*)0);
	__asm__ volatile (
        "LDR x10, =latency\n"  
        "BR x10\n"           
    );
	NOPS(15);
	cycle = cnt & 0xFF;
	
	NOPS(40);
	pick = reloadbuffer[cycle << 12];


    return 0;
}
