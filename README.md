# runahead behavior

## execution window size: about 1125
This is completed.
Still very interesting to check whether the execution window is depended on latency of load(using chaining to cause longer latency), or the execution windows is fixed?
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

### How does denver treat specific instructions

such as:
```
isb //wait all instructions finished, then execute next one
```

 
