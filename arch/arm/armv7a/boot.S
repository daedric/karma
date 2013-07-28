.global kmain

.arm

.comm stack, 0x10000 @ Reserve 64k stack in the BSS

_start:
.globl _start
    ldr r0, =__kernel_entry_point__   @ r0 = u-boot entry point
    ldr r1, =__ram_start_address__    @ r1 = destination where the kernel has to be moved
    ldr r2, =__kernel_end_mapping__   @ r2 = end
.loop:
    ldmia r0!, {r3-r12}
    stmia r1!, {r3-r12}
    cmp r1, r2
    blo .loop
    ldr sp, =stack+0x10000 @ Set up the stack
    ldr pc, =kmain
