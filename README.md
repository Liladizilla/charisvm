# CharisVM

CharisVM is an educational CPU emulator written from scratch in C. It starts with a small custom 8-bit architecture to make instruction execution, memory, and testing approachable.

## Status
Early development. This initial commit establishes the build, CI, a minimal CPU core, and automated tests. This is not an x86 emulator.

## Initial architecture
- Four 8-bit registers: R0–R3
- 256 bytes of RAM
- 8-bit address space with a 16-bit program-counter representation (to detect one-past-end fetches), plus a zero flag
- Fetch/decode/execute loop
- Bounded execution to prevent runaway programs

## Build and test
Requirements: GCC or Clang, GNU Make, and a POSIX-like shell.

    make
    make test
    make sanitize

## Development principles
- Keep behavior deterministic and tests reproducible.
- Treat malformed programs and invalid memory access as errors.
- Use strict compiler warnings and sanitizers in CI.
- Document limitations honestly.

## Roadmap
1. [x] Repository, build, and CI baseline
2. [ ] Define and test CPU state and instruction semantics
3. [ ] Implement strict program loader and CLI runner
4. [ ] Add assembler and sample programs
5. [ ] Add debugger, tracing, and memory inspection
6. [ ] Expand the architecture only after v0 is stable

## License
MIT. See LICENSE.