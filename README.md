# runahead behavior

## execution window size:
Still very interesting to check whether the execution window is depended on latency of load(using chaining to cause longer latency), or the execution windows is fixed?

Some discoverings: 
if merely use `if (index < size)`, the execution window is about `1134 nops + reload operation`	or `1000 adds + reload operation` or `4 divs + reload operation`. 

It seems `sdiv` instruction's behavior is strange.
## data dependency
Runahead will execute data dependent instructions, not merely execute load to warm cache.
```		
	/*
	* In runahead mode, index++ will be executed.
	*/
    if (index < ***index_p3)
    {
        index ++; 
        pick = reloadbuffer[fake_buffer[index] << 12];
    }
```

## potencial different data/control flow vs spectre

### nested branch prediction
How runahead treat nested branch maybe different from spectre out of order execution.

### function call and ret
Will runahead execution function calls?
How does runahead treat `ret` or `bl` instruction?

### data depending on loaded value

```
value = 100
flush(&value)
ISB
reloadbuffer[value * stride]
```
It possible that there will be two cache hit value: a fake one (from microarchitecture execution) and a true one (from architecture execution).

```
 54     memset(buf, 0x11, sizeof(char) * 4096);
 55     cacheflush(&buf);
 56     barrier();
 57     for(volatile int z = 0; z < 100; z++){
 58 
 59     }
 60     barrier();
 61     //pick = reloadbuffer[0x90 << 12];
 62     *((volatile char*)0);
 63     while(1){
 64         pick = reloadbuffer[0x88 << 12];
 65         pick = reloadbuffer[*buf << 12];
 66         pick = reloadbuffer[0x66 << 12];

```
Most of time, we see signal: 0x88, 0x66. But sometimes (1%) we see 0x88, 0x11, 0x66

### How does denver treat specific instructions

such as:
```
isb //wait all instructions finished, then execute next one
```

 
