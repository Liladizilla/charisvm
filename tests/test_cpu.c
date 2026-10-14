#include "cpu.h"

#include <assert.h>
#include <stdio.h>

static void test_load_add_output_and_halt(void)
{
    const uint8_t program[] = {
        OP_LDI, 0u, 7u, OP_LDI, 1u, 5u, OP_ADD, 0u, 1u,
        OP_OUT, 0u, OP_HLT
    };
    Cpu cpu;
    size_t steps = 0u;
    cpu_init(&cpu);
    assert(cpu_load_program(&cpu, program, sizeof(program)) == CPU_OK);
    assert(cpu_run(&cpu, 20u, &steps) == CPU_OK);
    assert(cpu.registers[0] == 12u);
    assert(cpu.output_written && cpu.output == 12u);
    assert(cpu.halted && steps == 5u);
}

static void test_zero_flag_and_conditional_jump(void)
{
    const uint8_t program[] = {
        OP_LDI, 0u, 0u, OP_JZ, 7u, OP_LDI, 1u, 99u, OP_HLT
    };
    Cpu cpu;
    cpu_init(&cpu);
    assert(cpu_load_program(&cpu, program, sizeof(program)) == CPU_OK);
    assert(cpu_run(&cpu, 10u, NULL) == CPU_OK);
    assert(cpu.registers[1] == 0u);
}

static void test_step_limit(void)
{
    const uint8_t program[] = { OP_JMP, 0u };
    Cpu cpu;
    size_t steps = 0u;
    cpu_init(&cpu);
    assert(cpu_load_program(&cpu, program, sizeof(program)) == CPU_OK);
    assert(cpu_run(&cpu, 4u, &steps) == CPU_ERR_STEP_LIMIT);
    assert(steps == 4u);
}

static void test_invalid_opcode(void)
{
    const uint8_t program[] = { 0xFFu };
    Cpu cpu;
    cpu_init(&cpu);
    assert(cpu_load_program(&cpu, program, sizeof(program)) == CPU_OK);
    assert(cpu_step(&cpu) == CPU_ERR_INVALID_OPCODE);
}

int main(void)
{
    test_load_add_output_and_halt();
    test_zero_flag_and_conditional_jump();
    test_step_limit();
    test_invalid_opcode();
    puts("All CharisVM tests passed.");
    return 0;
}