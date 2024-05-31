## output
```
```
➜  depends git:(main) ✗ ./init.sh
ITER + 1


complete!


ITER + 2


complete!


ITER + 3


complete!


ITER + 4


complete!


ITER + 5


complete!


ITER + 6
18 1c 23 29 34 3e 47 48 4c 4d 53 5b 5c 64 65 6d 6e 77 80 84 85 8e 97 98 a7 af b6 c7 ce de df f0 f6 fe ff 
) 4 > G H L M S [ \ d e m n w � � � � � � � � � � � � � � � � � 
complete!


ITER + 7
90 94 99 a6 ac ad b5 b6 ba cd d4 db e2 e3 e9 f2 f9 ff 
� � � � � � � � � � � � � � � � � � 
complete!


ITER + 8


complete!


ITER + 9
76 79 85 8a 8f 9b a0 a5 ab b6 bc c8 cf d5 d9 dd e7 ed f3 f9 fa fd 
v y � � � � � � � � � � � � � � � � � � � � 
complete!


ITER + 10


complete!


ITER + 11
91 94 9b a2 a6 b4 b7 be c5 cb d6 da dc df e5 f1 f7 fd 
� � � � � � � � � � � � � � � � � � 
complete!


ITER + 12
1f 21 25 2c 35 3b 43 49 4e 59 5e 63 6a 6e 73 77 80 86 8c 8d 95 98 9c 9d ac b2 b7 b8 c4 cc d2 d7 dd e2 e3 e8 f3 
 ! % , 5 ; C I N Y ^ c j n s w � � � � � � � � � � � � � � � � � � � � � 
complete!


ITER + 13


complete!


ITER + 14


complete!


ITER + 15
1d 21 2d 32 33 37 3e 43 49 4a 4e 4f 54 55 59 5f 60 77 7c 7d 84 8a 8f 98 9e a5 b5 bc c2 c3 c8 d6 dd e6 ed f5 fc 
 ! - 2 3 7 > C I J N O T U Y _ ` w | } � � � � � � � � � � � � � � � � � 
complete!


ITER + 16


complete!

```
 78		pick = 0x66;
 79     memset(buf, 0x11, sizeof(unsigned char) * 4096);
 80     cacheflush(buf);
 81     barrier();
 82     *((volatile char*)0); //illegal mem access
 83     pick = *buf;  //cachemiss access
 84     NOPS(N);	  //nops, 105
 85     pick = reloadbuffer[pick << 12]; 	//check missing data dependencies old value:0x66, new: 0x11
 86     pick = reloadbuffer[0x88 << 12];	//normal load
```
