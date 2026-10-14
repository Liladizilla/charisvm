#include "cpu.h"

#include <stdio.h>

static const char *status_name(CpuStatus status)
{
    switch (status) {
    case CPU_OK: return "ok";
    case CPU_ERR_INVALID_ARGUMENT: return "invalid argument";
    case CPU_ERR_INVALID_OPCODE: return "invalid opcode";
    case CPU_ERR_PROGRAM_BOUNDS: return "program bounds";
    case CPU_ERR_STEP_LIMIT: return "step limit";
    }
    return "unknown";
}

static void dump(const Cpu *cpu, size_t step, const char *event)
{
    printf("[%03zu] %-8s PC=%03u  R0=%02X R1=%02X R2=%02X R3=%02X  Z=%u  OUT=",
           step, event, (unsigned)cpu->pc,
           (unsigned)cpu->registers[0], (unsigned)cpu->registers[1],
           (unsigned)cpu->registers[2], (unsigned)cpu->registers[3],
           cpu->zero_flag ? 1u : 0u);
    if (cpu->output_written) printf("%02X", (unsigned)cpu->output);
    else printf("--");
    putchar('\n');
}

int main(int argc, char **argv)
{
    FILE *file;
    unsigned value;
    uint8_t program[CPU_RAM_SIZE];
    size_t length = 0u, steps = 0u;
    Cpu cpu;
    CpuStatus status;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <program.hex>\n", argv[0]);
        return 2;
    }
    file = fopen(argv[1], "r");
    if (file == NULL) {
        perror(argv[1]);
        return 2;
    }
    while (fscanf(file, "%x", &value) == 1) {
        if (value > 0xFFu || length >= sizeof(program)) {
            fprintf(stderr, "charisvm-debug: invalid byte or program too large\n");
            fclose(file);
            return 2;
        }
        program[length++] = (uint8_t)value;
    }
    if (ferror(file)) {
        perror("read");
        fclose(file);
        return 2;
    }
    fclose(file);
    if (length == 0u) {
        fprintf(stderr, "charisvm-debug: empty program\n");
        return 2;
    }

    cpu_init(&cpu);
    status = cpu_load_program(&cpu, program, length);
    if (status != CPU_OK) {
        fprintf(stderr, "charisvm-debug: load: %s\n", status_name(status));
        return 1;
    }
    dump(&cpu, 0u, "START");
    while (!cpu.halted && steps < 10000u) {
        status = cpu_step(&cpu);
        if (status != CPU_OK) {
            dump(&cpu, steps, "ERROR");
            fprintf(stderr, "charisvm-debug: %s after %zu instruction(s)\n",
                    status_name(status), steps);
            return 1;
        }
        steps++;
        dump(&cpu, steps, cpu.halted ? "HALT" : "STEP");
    }
    if (!cpu.halted) {
        fprintf(stderr, "charisvm-debug: step limit reached\n");
        return 1;
    }
    printf("Debug session finished: %zu instruction(s).\n", steps);
    return 0;
}
