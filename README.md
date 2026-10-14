# CharisVM

An educational CPU emulator written from scratch in C. The first version implements a small custom 8-bit machine to make instruction execution and memory behavior easy to inspect. It is **not** an x86 emulator.

## Requirements

- GCC or Clang
- GNU Make
- POSIX-like shell

## Build, test, run

```sh
make
make test
make sanitize
./build/charisvm programs/sum.hex
```

The example program calculates 7 + 5. The emulator prints the final registers, program counter, zero flag, and output latch.

## Hex program format

Programs are text files containing hexadecimal byte values separated by whitespace or commas. A `#` starts a comment. Prefix bytes with `0x` if desired.

Example:

```text
10 00 07  # LDI R0, 7
10 01 05  # LDI R1, 5
20 00 01  # ADD R0, R1
F0 00     # OUT R0
00        # HLT
```

## Architecture

- Four 8-bit registers: R0–R3
- 256 bytes of flat RAM
- Program counter with one-past-end detection
- Zero flag
- Bounded execution (10,000 instructions in the CLI)
- Strict compiler warnings and sanitizer CI

See [docs/architecture.md](docs/architecture.md) for opcodes and current limitations.

## Development

```sh
make clean
make
make test
make sanitize
```

Changes should include tests. GitHub Actions builds the CLI, runs unit tests, and runs AddressSanitizer/UndefinedBehaviorSanitizer checks.

## Roadmap

- [x] CPU core and instruction semantics
- [x] CI build/test/sanitizer baseline
- [x] Hex program loader and CLI register/output display
- [ ] Assembler and sample programs
- [ ] Instruction tracing and debugger
- [ ] Memory inspection and more complete error reporting
