#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>
#include "utils.h"
#ifndef N
#define N 105
#endif

#define STRIDE 4096
#define RELOAD_BUF_SIZE STRIDE * 256
#define ITER 1024
#define NOPS(n) \
    asm volatile( \
        ".rept %0\n" \
        "nop\n" \
        ".endr\n" \
        :: "i" (n) \
    )

static unsigned char buf[4096] __attribute__((aligned(4096))) = {0x99};
uint8_t *reloadbuffer;
uint64_t measured_clock;
struct timespec start, end;
int result[256];
int idx;
int flag = 0;

void handler(int sig)
{
	//printf("signal handler\n");
	for(int i = 0; i < 256; i++){
		//idx = ((i * 167) + 13) & 255;
		idx = i;
		measured_clock = timed_read(&reloadbuffer[idx * STRIDE], &start, &end);
		if(measured_clock <= 160){
	//		if(idx == 0x88 || idx == 0x11)
				//printf("%c %x ", idx, idx);
			result[idx] = 1;
		}
	}
	for(int i = 0; i < 256; i++){
		if(result[i] == 1){
			printf("%x ", i);
		}
	}
	printf("\n");
	for(int i = 0; i < 256; i++){
		if(result[i] == 1){
			printf("%c ", i);
		}
	}
	
	printf("\ncomplete!\n");
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
	memset(result, 0, sizeof(int) * 256);
	register volatile uint8_t pick = 0;
	

	
	//flush the reloadbuffer
	for(int i = 0; i < 256; i++){
		cacheflush(&reloadbuffer[STRIDE * i]);
	}

	barrier();
	pick = 0x66;
	memset(buf, 0x11, sizeof(unsigned char) * 4096);
	cacheflush(buf);
	barrier();
	*((volatile char*)0);
	pick = *buf;
	NOPS(N);
	pick = reloadbuffer[pick << 12];
	pick = reloadbuffer[0x88 << 12];
}
