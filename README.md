# CharisVM

CharisVM currently has two distinct pieces:

1. **CharisVM-8**, a small educational 8-bit CPU emulator written in C. It is useful for learning instruction sets, registers, RAM, assemblers, and debugging.
2. **CharisVM-QEMU**, a launcher for a real x86-64 guest machine backed by QEMU and, when available, Linux KVM hardware virtualization.

The C emulator is not an x86 emulator or a hypervisor. The QEMU launcher is the path for booting an actual guest operating system on your computer's virtual hardware. Keeping those roles separate avoids pretending that a toy ISA is somehow running Linux.

## Build and test the 8-bit emulator

Requirements: GCC or Clang, GNU Make, and a POSIX shell.

```sh
make
make test
make sanitize
./build/charisasm programs/sum.asm build/sum.hex
./build/charisvm build/sum.hex
./build/charisvm-debug build/sum.hex
```

The example calculates 7 + 5. The CLI prints the final registers, program counter, zero flag, and output latch. The debugger traces instruction execution and displays RAM snapshots at startup and on errors.

## Run a real x86-64 virtual machine

On Fedora, install QEMU:

```sh
sudo dnf install qemu-kvm qemu-img
```

Get a bootable installer ISO for the guest operating system you want to install, then launch it:

```sh
make vm VM_ARGS="--iso ~/Downloads/installer.iso"
```

The launcher creates a 20 GiB QCOW2 virtual disk at `~/.local/share/charisvm/guest.qcow2` on first launch. Replace the example ISO path with the actual path on your machine. Install the guest OS onto that virtual disk inside the QEMU window.

After installation, launch the installed guest without the installer ISO:

```sh
make vm
```

Useful options:

```sh
make vm VM_ARGS="--iso ~/Downloads/installer.iso --memory 4096 --cpus 4"
make vm VM_ARGS="--disk ~/VMs/dev.qcow2 --memory 2048 --cpus 2"
```

The default disk is persistent: the guest OS and its files remain on the disk between launches. Back up important guest data like you would any other computer.

### Acceleration and requirements

- If `/dev/kvm` is readable and writable, the launcher selects KVM hardware acceleration.
- Otherwise, it uses QEMU's software TCG accelerator. That is slower, but does not require KVM access.
- KVM requires CPU virtualization support enabled in firmware and suitable Linux permissions.
- This first launcher is Linux-hosted and expects `qemu-system-x86_64` and `qemu-img` on `PATH`.
- It does not download operating systems, bypass licensing, or implement its own hypervisor. You provide the installer ISO and QEMU supplies the virtual hardware.

## The 8-bit assembly language

The `charisasm` tool accepts one instruction per line, comments beginning with `#`, and case-insensitive labels ending in a colon.

```asm
start:
    LDI R0, 7
    LDI R1, 5
    ADD R0, R1
    OUT R0
    HLT
```

Supported instructions: `HLT`, `LDI`, `MOV`, `ADD`, `SUB`, `LOAD`, `STORE`, `JMP`, `JZ`, and `OUT`. Numeric operands may be decimal or prefixed with `0x`. Branch targets are absolute byte addresses.

## Architecture and limitations

- Four 8-bit registers (R0–R3), 256 bytes of flat RAM, a zero flag, and an output latch.
- Code and data share the same RAM in the educational emulator.
- Arithmetic wraps modulo 256; carry and overflow flags are not modeled.
- The QEMU guest uses x86-64 virtual hardware; it is not executed by the custom 8-bit CPU core.

See [docs/architecture.md](docs/architecture.md) for the emulator's instruction encoding and [docs/qemu-vm.md](docs/qemu-vm.md) for the real VM launcher details.

## Development

```sh
make clean
make
make test
make sanitize
```

Changes should include tests. GitHub Actions compiles with strict warnings, runs the unit and integration tests, and checks memory safety with AddressSanitizer and UndefinedBehaviorSanitizer.
