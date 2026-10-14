# CharisVM v0 Architecture

## Machine state
- Four 8-bit general-purpose registers (R0–R3).
- 256-byte flat RAM shared by instructions and data.
- 8-bit branch addresses and a 16-bit program-counter representation, allowing the emulator to detect one-past-end fetches.
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
- The current loader tracks the loaded program length and rejects instruction/operand fetches beyond it. Code and data still share flat RAM, so STORE can modify instruction bytes within the loaded range.
- Branch targets are absolute 8-bit addresses.
- Arithmetic wraps modulo 256; carry and overflow flags are not modeled.
- The output latch is not yet printed by the CLI.
- This is an educational custom ISA, not x86/x86-64.

Limitations are documented explicitly so the project does not overstate its capabilities.