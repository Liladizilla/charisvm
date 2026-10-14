#ifndef CHARISVM_CPU_H
#define CHARISVM_CPU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define CPU_REGISTER_COUNT 4u
#define CPU_RAM_SIZE 256u

typedef enum {
    CPU_OK = 0,
    CPU_ERR_INVALID_ARGUMENT,
    CPU_ERR_INVALID_OPCODE,
    CPU_ERR_PROGRAM_BOUNDS,
    CPU_ERR_STEP_LIMIT
} CpuStatus;

typedef enum {
    OP_HLT = 0x00,
    OP_LDI = 0x10,
    OP_MOV = 0x11,
    OP_ADD = 0x20,
    OP_SUB = 0x21,
    OP_LOAD = 0x30,
    OP_STORE = 0x31,
    OP_JMP = 0x40,
    OP_JZ = 0x41,
    OP_OUT = 0xF0
} CpuOpcode;

typedef struct {
    uint8_t registers[CPU_REGISTER_COUNT];
    uint8_t ram[CPU_RAM_SIZE];
    uint8_t pc;
    bool zero_flag;
    bool halted;
    uint8_t output;
    bool output_written;
} Cpu;

void cpu_init(Cpu *cpu);
CpuStatus cpu_load_program(Cpu *cpu, const uint8_t *program, size_t length);
CpuStatus cpu_step(Cpu *cpu);
CpuStatus cpu_run(Cpu *cpu, size_t max_steps, size_t *steps_executed);

#endif