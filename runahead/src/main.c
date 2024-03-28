#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <time.h>
#include <stdint.h>	
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"



int main(){
	struct timespec start, end;
	uint64_t time;
	char *reloadbuffer = (unsigned char *)mmap(0, RELOAD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE | MAP_HUGETLB, -1, 0);
    memset(reloadbuffer, 2, RELOAD_BUF_SIZE);
	PREFETCH prefetch_res[probe_size];
	volatile char pick;
	for(int j = 0; j < iter; j++){
		for(int i = 0 ; i < probe_size; i++){
			flush(reloadbuffer + i);
		}
		pick = reloadbuffer[0];
		for(int i = 0 ; i < probe_size; i++){
			clock_init(&start);
			pick = reloadbuffer[i];
			clock_end(&end);
			time = get_clock(start, end);
			prefetch_res[i].result[j] = time;
			//printf("i: %d addr: 0x%p, time: %llu\n", i, reloadbuffer + i, (long long unsigned int) time);
		}
	}
	for(int i = 0; i < probe_size; i = i + 1){
		qsort(prefetch_res[i].result, 5, sizeof(uint64_t), cmp_uint64);

		if(prefetch_res[i].result[iter / 2] > 100)
			printf("i: %d addr: 0x%p, time: %llu\n", i, reloadbuffer + i * 64, (long long unsigned int) prefetch_res[i].result[iter / 2]);
	}

	/*measure time for cache hit*/
//    for(int i = 0; i < 1024; i++){
//	  	for(int j = 0; j < 10; j++){
//	  		pick = reloadbuffer[i * STRIDE];
//    		clock_init(&start);
//	  		pick = reloadbuffer[i * STRIDE];
//    		clock_end(&end);
//    		time = get_clock(start, end);
//	  		printf("cache_hit,time,%llu\n", (long long unsigned int) time);
//	  	}
//  	}
  

//  	for(int i = 0; i < 1024; i++){
//	  	for(int j = 0; j < 10; j++){
//	  		flush(reloadbuffer + i * STRIDE);
//    		clock_init(&start);
//	  		pick = reloadbuffer[i * STRIDE];
//    		clock_end(&end);
//    		time = get_clock(start, end);
//	  		printf("cache_miss,time,%llu\n", (long long unsigned int) time);
//	  }
//  	}
    return 0;
}
