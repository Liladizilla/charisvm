# Running a real x86-64 guest

CharisVM's QEMU launcher uses QEMU's x86-64 machine model. It does not run x86 instructions inside the custom 8-bit emulator.

## How it works

- QEMU provides virtual CPU, chipset, display, storage, and boot devices.
- Linux KVM accelerates guest execution when `/dev/kvm` is accessible.
- QEMU TCG is used as a software fallback when KVM cannot be used.
- The guest's persistent virtual disk is a QCOW2 image. The launcher creates a 20 GiB virtual disk when the selected disk does not yet exist.
- An ISO is optional for an existing disk, but required to create a new disk because a guest OS must be installed.

## Example

```sh
sudo dnf install qemu-kvm qemu-img
sh scripts/charisvm-qemu --iso /path/to/linux-installer.iso --memory 4096 --cpus 4
```

Complete the OS installation in the QEMU display. Eject/remove the installer from the guest workflow, shut down the guest, and launch again without `--iso` to boot from the installed disk.

## Safety and current scope

Use trusted guest OS images. The guest is a separate operating system, but it still shares host resources through QEMU. Do not pass through host disks or sensitive devices unless you understand the security implications.

This is an initial launcher, not a full VM management suite. It does not yet provide snapshots, a GUI manager, network configuration controls, USB passthrough, or an independently implemented hypervisor. The custom 8-bit emulator remains a separate educational component.
