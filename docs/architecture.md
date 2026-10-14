# CharisVM-8 architecture

CharisVM-8 is the educational custom CPU emulator. The separate QEMU launcher can start a real x86-64 guest OS; see [qemu-vm.md](qemu-vm.md).

## Machine state

- Four 8-bit general-purpose registers (R0–R3).
- 256-byte flat RAM shared by program instructions and data.
- 16-bit program-counter representation; branch addresses are 8-bit absolute byte addresses.
- One zero flag, halt state, and one-byte output latch.
- CLI loads a hexadecimal byte file and limits execution to 10,000 instructions.

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

## Assembler and debugger

- `charisasm` converts the documented assembly language to hexadecimal byte files.
- Labels are case-insensitive; unresolved and duplicate labels are rejected.
- `charisvm-debug` traces instruction steps, registers, PC, zero flag, output latch, and selected RAM snapshots.
- Integration tests exercise assembler output, control flow, debugger output, invalid operands, unresolved labels, and QEMU launcher argument validation.

## Known limitations

- This is not an x86/x86-64 emulator and does not itself implement a hardware hypervisor.
- Program code and data share the same 256-byte RAM; a `STORE` can alter instruction bytes.
- Arithmetic wraps modulo 256; carry and overflow flags are not modeled.
- Branch targets are absolute 8-bit addresses, limiting direct control flow to the first 256 bytes.
- The QEMU launcher is currently Linux-hosted and relies on external QEMU/KVM; it is not yet a complete VM manager.
