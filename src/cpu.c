#include "cpu.h"

#include <string.h>

void cpu_init(Cpu *cpu)
{
    if (cpu != NULL) {
        memset(cpu, 0, sizeof(*cpu));
    }
}

CpuStatus cpu_load_program(Cpu *cpu, const uint8_t *program, size_t length)
{
    if (cpu == NULL || (program == NULL && length != 0u)) {
        return CPU_ERR_INVALID_ARGUMENT;
    }
    if (length > CPU_RAM_SIZE) {
        return CPU_ERR_PROGRAM_BOUNDS;
    }
    memset(cpu->ram, 0, sizeof(cpu->ram));
    if (length != 0u) {
        memcpy(cpu->ram, program, length);
    }
    cpu->pc = 0u;
    cpu->halted = false;
    cpu->output_written = false;
    return CPU_OK;
}

static CpuStatus fetch_byte(Cpu *cpu, uint8_t *value)
{
    if ((size_t)cpu->pc >= CPU_RAM_SIZE) {
        return CPU_ERR_PROGRAM_BOUNDS;
    }
    *value = cpu->ram[cpu->pc];
    cpu->pc = (uint8_t)(cpu->pc + 1u);
    return CPU_OK;
}

static CpuStatus fetch_operand(Cpu *cpu, uint8_t *value)
{
    return fetch_byte(cpu, value);
}

CpuStatus cpu_step(Cpu *cpu)
{
    uint8_t opcode, a, b;
    CpuStatus status;

    if (cpu == NULL) return CPU_ERR_INVALID_ARGUMENT;
    if (cpu->halted) return CPU_OK;

    status = fetch_byte(cpu, &opcode);
    if (status != CPU_OK) return status;

    switch (opcode) {
    case OP_HLT:
        cpu->halted = true;
        return CPU_OK;
    case OP_LDI:
        if ((status = fetch_operand(cpu, &a)) != CPU_OK ||
            (status = fetch_operand(cpu, &b)) != CPU_OK) return status;
        if (a >= CPU_REGISTER_COUNT) return CPU_ERR_INVALID_ARGUMENT;
        cpu->registers[a] = b;
        cpu->zero_flag = (b == 0u);
        return CPU_OK;
    case OP_MOV:
        if ((status = fetch_operand(cpu, &a)) != CPU_OK ||
            (status = fetch_operand(cpu, &b)) != CPU_OK) return status;
        if (a >= CPU_REGISTER_COUNT || b >= CPU_REGISTER_COUNT)
            return CPU_ERR_INVALID_ARGUMENT;
        cpu->registers[a] = cpu->registers[b];
        cpu->zero_flag = (cpu->registers[a] == 0u);
        return CPU_OK;
    case OP_ADD:
    case OP_SUB:
        if ((status = fetch_operand(cpu, &a)) != CPU_OK ||
            (status = fetch_operand(cpu, &b)) != CPU_OK) return status;
        if (a >= CPU_REGISTER_COUNT || b >= CPU_REGISTER_COUNT)
            return CPU_ERR_INVALID_ARGUMENT;
        if (opcode == OP_ADD)
            cpu->registers[a] = (uint8_t)(cpu->registers[a] + cpu->registers[b]);
        else
            cpu->registers[a] = (uint8_t)(cpu->registers[a] - cpu->registers[b]);
        cpu->zero_flag = (cpu->registers[a] == 0u);
        return CPU_OK;
    case OP_LOAD:
    case OP_STORE:
        if ((status = fetch_operand(cpu, &a)) != CPU_OK ||
            (status = fetch_operand(cpu, &b)) != CPU_OK) return status;
        if (a >= CPU_REGISTER_COUNT) return CPU_ERR_INVALID_ARGUMENT;
        if (opcode == OP_LOAD) {
            cpu->registers[a] = cpu->ram[b];
            cpu->zero_flag = (cpu->registers[a] == 0u);
        } else {
            cpu->ram[b] = cpu->registers[a];
        }
        return CPU_OK;
    case OP_JMP:
    case OP_JZ:
        if ((status = fetch_operand(cpu, &a)) != CPU_OK) return status;
        if (opcode == OP_JMP || cpu->zero_flag) cpu->pc = a;
        return CPU_OK;
    case OP_OUT:
        if ((status = fetch_operand(cpu, &a)) != CPU_OK) return status;
        if (a >= CPU_REGISTER_COUNT) return CPU_ERR_INVALID_ARGUMENT;
        cpu->output = cpu->registers[a];
        cpu->output_written = true;
        return CPU_OK;
    default:
        return CPU_ERR_INVALID_OPCODE;
    }
}

CpuStatus cpu_run(Cpu *cpu, size_t max_steps, size_t *steps_executed)
{
    size_t steps = 0u;
    CpuStatus status;

    if (steps_executed != NULL) *steps_executed = 0u;
    if (cpu == NULL || max_steps == 0u) return CPU_ERR_INVALID_ARGUMENT;

    while (!cpu->halted && steps < max_steps) {
        status = cpu_step(cpu);
        if (status != CPU_OK) {
            if (steps_executed != NULL) *steps_executed = steps;
            return status;
        }
        steps++;
    }
    if (steps_executed != NULL) *steps_executed = steps;
    return cpu->halted ? CPU_OK : CPU_ERR_STEP_LIMIT;
}