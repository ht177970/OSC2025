#pragma once
#include <stdint.h>

int fdt_check_header(const void *fdt);

const char* fdt_get_model(const void *fdt);

uint64_t fdt_get_uart_base(const void *fdt);

