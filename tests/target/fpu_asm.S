.syntax unified
.cpu cortex-m7
.thumb

/*Target is to test 32 hardware registers without the compiler throwing the state under the carpet*/

.global fpu_fill_pattern

.type fpu_fill_pattern, %function

fpu_fill_pattern:

.irp reg, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        
vmov    s\reg, r0
adds    r0, r0, #1
    
.endr
    
bx lr

.global fpu_verify_pattern

.type fpu_verify_pattern, %function

fpu_verify_pattern:
 
mov     r1, r0
.irp reg, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
        
vmov    r2, s\reg
cmp     r2, r1
bne     .Lfpu_mismatch_\reg
adds    r1, r1, #1

.endr
mov     r0, #0


bx  lr

.irp reg, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31

.Lfpu_mismatch_\reg:
        
mov     r0, #(\reg + 1)
        
bx   lr
    
.endr
