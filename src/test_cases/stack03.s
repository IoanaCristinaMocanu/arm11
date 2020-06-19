mov r1,#5
add r2,r1,#10
mul r3,r1,r2
mov r7,r3
func:
stmed sp!,{r1,r2,r3}
mul r1,r7,r7
mul r2,r1,#3
mov r6,r2
ldmed sp!,{r1,r2,r3}
main:
mul r5,r1,#2
