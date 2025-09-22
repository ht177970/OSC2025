#pragma once

#define ul unsigned long

struct sbiret {
    long error;
    long value;
};

struct sbiret sbi_ecall(int ext, int fid, ul arg0, ul arg1, ul arg2,
                        ul arg3, ul arg4, ul arg5);
