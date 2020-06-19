ldr r1,=0x11111111
ldr r2,=0x22222222
ldr r3,=0x3fffffff
stmed sp!,{r1,r2,r3}
ldmed sp!,{r5,r6,r7}
