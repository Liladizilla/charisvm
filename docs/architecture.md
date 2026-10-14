# CharisVM v0 Architecture

## Machine state
- Four 8-bit general-purpose registers (R0–R3).
- 256-byte flat RAM shared by instructions and data.
- 8-bit program counter.
- One zero flag, halt state, and one-byte output latch.

## Instruction encoding
Instructions are byte-oriented. The opcode byte is followed by operand bytes.

| Opcode | Name | Operands | Semantics |
|---:|---|---|---|
| 0x00 | HLT | none | Halt CPU |
| 0x10 | LDI | register, immediate | Load immediate |
| 0x11 | MOV | destination, source | Copy register |
| 0x20 | ADD | destination, source | 8-bit wrapping addition |
| 0x21 | SUB | destination, source | 8-bit wrapping subtraction |
| 0x30 | LOAD | register, address | Read RAM |
| 0x31 | STORE | register, address | Write RAM |
| 0x40 | JMP | address | Absolute jump |
| 0x41 | JZ | address | Jump when zero flag is set |
| 0xF0 | OUT | register | Write output latch |

## Known limitations
- The CLI does not yet load programs from files.
- Flat RAM means fetch currently reads zero-filled memory beyond the loaded byte sequence. Strict loaded-program bounds and malformed-instruction detection are required before the core is stable.
- Branch targets are absolute 8-bit addresses.
- Arithmetic wraps modulo 256; carry and overflow flags are not modeled.
- The output latch is not yet printed by the CLI.
- This is an educational custom ISA, not x86/x86-64.

Limitations are documented explicitly so the project does not overstate its capabilities.