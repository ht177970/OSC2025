.section ".text"

.global _start

_start:
    la sp, __stack_top
    mv a0, a1
    la t0, __bss_start
    la t1, __bss_end

clear_bss:
    bge t0, t1, boot
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss

boot:
    call run_kernel

lp:
    wfi
    j lp
