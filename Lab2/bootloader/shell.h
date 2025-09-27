#pragma once
#include <stdint.h>

extern unsigned long uart_base;
extern const char* model_name;

void run_shell(uint64_t dtb);
