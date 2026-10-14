#include "cpu.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STEPS 10000u

static void print_usage(const char *name)
{
    fprintf(stderr, "Usage: %s <program.hex>\n", name);
    fprintf(stderr, "Program files contain hexadecimal bytes; # starts a comment.\n");
}

static int load_hex_file(const char *path, uint8_t *buffer, size_t capacity, size_t *length)
{
    FILE *file = fopen(path, "r");
    char line[512];
    size_t count = 0u;
    unsigned line_number = 0u;

    if (file == NULL) {
        fprintf(stderr, "charisvm: cannot open '%s': %s\n", path, strerror(errno));
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char *cursor = line;
        char *comment;
        char *token;
        line_number++;
        comment = strchr(cursor, '#');
        if (comment != NULL) *comment = '\0';

        token = strtok(cursor, " \t\r\n,");
        while (token != NULL) {
            char *end = NULL;
            unsigned long value;
            if (token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
                token += 2;
            if (*token == '\0') {
                token = strtok(NULL, " \t\r\n,");
                continue;
            }
            errno = 0;
            value = strtoul(token, &end, 16);
            if (errno != 0 || end == token || *end != '\0' || value > 0xFFul) {
                fprintf(stderr, "charisvm: invalid byte on line %u: '%s'\n",
                        line_number, token);
                fclose(file);
                return 0;
            }
            if (count >= capacity) {
                fprintf(stderr, "charisvm: program exceeds %zu-byte memory\n", capacity);
                fclose(file);
                return 0;
            }
            buffer[count++] = (uint8_t)value;
            token = strtok(NULL, " \t\r\n,");
        }
    }

    if (ferror(file)) {
        fprintf(stderr, "charisvm: error reading '%s'\n", path);
        fclose(file);
        return 0;
    }
    fclose(file);
    if (count == 0u) {
        fprintf(stderr, "charisvm: program is empty\n");
        return 0;
    }
    *length = count;
    return 1;
}

static const char *status_name(CpuStatus status)
{
    switch (status) {
    case CPU_OK: return "ok";
    case CPU_ERR_INVALID_ARGUMENT: return "invalid argument or register";
    case CPU_ERR_INVALID_OPCODE: return "invalid opcode";
    case CPU_ERR_PROGRAM_BOUNDS: return "instruction fetch outside loaded program";
    case CPU_ERR_STEP_LIMIT: return "execution step limit reached";
    }
    return "unknown CPU error";
}

int main(int argc, char **argv)
{
    uint8_t program[CPU_RAM_SIZE];
    size_t program_size = 0u;
    size_t steps = 0u;
    Cpu cpu;
    CpuStatus status;

    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (!load_hex_file(argv[1], program, sizeof(program), &program_size))
        return EXIT_FAILURE;

    cpu_init(&cpu);
    status = cpu_load_program(&cpu, program, program_size);
    if (status != CPU_OK) {
        fprintf(stderr, "charisvm: load failed: %s\n", status_name(status));
        return EXIT_FAILURE;
    }
    status = cpu_run(&cpu, MAX_STEPS, &steps);
    if (status != CPU_OK) {
        fprintf(stderr, "charisvm: execution failed after %zu step(s): %s\n",
                steps, status_name(status));
        return EXIT_FAILURE;
    }

    printf("CharisVM halted successfully after %zu instruction(s).\n", steps);
    for (size_t i = 0u; i < CPU_REGISTER_COUNT; i++)
        printf("R%zu = %u (0x%02X)\n", i, (unsigned)cpu.registers[i],
               (unsigned)cpu.registers[i]);
    printf("PC = %u\n", (unsigned)cpu.pc);
    printf("ZERO = %s\n", cpu.zero_flag ? "true" : "false");
    if (cpu.output_written)
        printf("OUT = %u (0x%02X)\n", (unsigned)cpu.output, (unsigned)cpu.output);
    return EXIT_SUCCESS;
}
