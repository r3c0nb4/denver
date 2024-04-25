##Runahead dependency

## disable flush
```
56     //cacheflush(&buf);
 57     pick = reloadbuffer[0x11 << 12];
 58     barrier();
 59     for(volatile int z = 0; z < 100; z++){
 60     
 61     }
 62     barrier();
 63     //pick = reloadbuffer[0x90 << 12];
 64     *((volatile char*)0);
 65     pick = reloadbuffer[*buf << 12];
 66     asm volatile(
 67         ".rept 65\n"
 68         "nop\n"
 69         ".endr\n"
 70     );
 71     pick = reloadbuffer[0x88 << 12];
```
After line 64 which causes error, the first cache miss is `reloadbuff[0x88 << 12]`.
In this case, most of time, we just see signal `0x11` cache hit.

## enable flush

```
 56     cacheflush(&buf);
 57     //pick = reloadbuffer[0x11 << 12];
 58     barrier();
 59     for(volatile int z = 0; z < 100; z++){
 60     
 61     }
 62     barrier();
 63     //pick = reloadbuffer[0x90 << 12];
 64     *((volatile char*)0);
 65     pick = reloadbuffer[*buf << 12];
 66     asm volatile(
 67         ".rept 65\n"
 68         "nop\n"
 69         ".endr\n"
 70     );
 71     pick = reloadbuffer[0x88 << 12];
 72     //pick = reloadbuffer[0x66 << 12];

```
We change the code a little bit, this time `buff` is not in the cache. We will see more `0x88` cache hit in the signal, however, we do not see any `0x11`. 

### Output

