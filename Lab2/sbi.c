#include "sbi.h"
#include <stdint.h>

#define WARM_REBOOT 0x2
#define NO_REASON 0x0

struct sbiret sbi_ecall(int ext, int fid, ul arg0, ul arg1, ul arg2,
                        ul arg3, ul arg4, ul arg5)
{
    struct sbiret ret;

    register uintptr_t a0 asm ("a0") = (uintptr_t)(arg0);
	register uintptr_t a1 asm ("a1") = (uintptr_t)(arg1);
	register uintptr_t a2 asm ("a2") = (uintptr_t)(arg2);
	register uintptr_t a3 asm ("a3") = (uintptr_t)(arg3);
	register uintptr_t a4 asm ("a4") = (uintptr_t)(arg4);
	register uintptr_t a5 asm ("a5") = (uintptr_t)(arg5);
	register uintptr_t a6 asm ("a6") = (uintptr_t)(fid);
	register uintptr_t a7 asm ("a7") = (uintptr_t)(ext);
	asm volatile ("ecall"
		      : "+r" (a0), "+r" (a1)
		      : "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7)
		      : "memory");
	ret.error = a0;
	ret.value = a1;

	return ret;
}

struct sbiret sbi_system_reboot(){
    return sbi_ecall(SBI_EXT_SRST , 0x0, WARM_REBOOT, NO_REASON, 0x0, 0x0, 0x0, 0x0);
}
