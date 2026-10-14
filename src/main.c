#include "cpu.h"

#include <stdio.h>

int main(void)
{
    Cpu cpu;
    cpu_init(&cpu);
    puts("CharisVM v0.1: CPU core initialized.");
    puts("Program loading and instruction tracing are the next milestone.");
    return 0;
}