#pragma once

#define ul unsigned long
#define SBI_EXT_SRST 0x53525354

struct sbiret {
    long error;
    long value;
};

struct sbiret sbi_ecall(int ext, int fid, ul arg0, ul arg1, ul arg2,
                        ul arg3, ul arg4, ul arg5);

struct sbiret sbi_system_reboot();
