#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <signal.h>
#include "utils.h"

#define STRIDE 4096
#define RELOAD_BUF_SIZE STRIDE * 256
#define ITER 1024

static char buf[4096] __attribute__((aligned(4096))) = {0x97};
uint8_t *reloadbuffer;
volatile uint8_t pick;
uint64_t measured_clock;
struct timespec start, end;
int result[256];
int idx;
int flag = 0;

void handler(int sig)
{
	printf("signal handler\n");
	for(int i = 0; i < 256; i++){
		idx = ((i * 167) + 13) & 255;
		measured_clock = timed_read(&reloadbuffer[idx * STRIDE], &start, &end);
		if(measured_clock <= 160){
			//if(idx == 0x90 || idx == 0x98)
				printf("cache hit: %x\n", idx);
		}
	}
	
	printf("exit\n");
    exit(0);
}

int main(){
	struct sigaction sa;
    memset (&sa, '\0', sizeof(sa));
    sa.sa_sigaction = &handler;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL); 
	reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	cacheflush(&buf);
	barrier();
	for(volatile int z = 0; z < 100; z++){

	}
	for(int i = 0; i < 1024; i++){
		pick = reloadbuffer[0x90 << 12];
	}
	*((volatile char*)0);
	while(1){
		pick = reloadbuffer[0x98 << 12];
		pick = reloadbuffer[*buf << 12];
	}
		
//		for (int i = 0; i < 256; i++)
//		{
//			index = ((i * 167) + 13) & 255;
//			measured_clock = timed_read(&reloadbuffer[index * STRIDE], &start, &end);
//			if (measured_clock <= 100){
//				result[index]++; 
//			}
//		}	
}

