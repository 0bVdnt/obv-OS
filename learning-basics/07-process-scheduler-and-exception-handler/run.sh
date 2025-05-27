#!/bin/bash
# Enable shell debugging and error handling
# -x: Print commands and their arguments as they are executed
# -u: Treat unset variables as errors
# -e: Exit immediately if a command returns a non-zero status
set -xue

# Specify the QEMU emulator for RISC-V 32-bit
QEMU=qemu-system-riscv32

# Define compiler and its flags
CC=/usr/bin/clang
# Compiler flags:
# -std=c11: Use C11 standard
# -O2: Optimize for speed
# -g3: Include maximum debugging information
# -Wall -Wextra: Enable all warnings
# --target=riscv32: Target RISC-V 32-bit architecture
# -ffreestanding: Compile for freestanding environment (no standard library)
# -nostdlib: Don't link against standard library
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# Build the kernel
# -Wl,-Tkernel.ld: Use kernel.ld as the linker script
# -Wl,-Map=kernel.map: Generate a memory map file
# -o kernel.elf: Output the executable as kernel.elf
# kernel.c common.c: Source files to compile
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c common.c
# Note: -Wl, means passing options to the linker instead of the C compiler.
#       clang command does C compilation and executes the linker internally.

# Start QEMU with the following options:
# -machine virt: Use QEMU's RISC-V virtual machine
# -bios default: Use default BIOS
# -nographic: Disable graphical output
# -serial mon:stdio: Redirect serial port to stdio with monitor
# --no-reboot: Don't reboot on system reset
# -kernel kernel.elf: Use our compiled kernel as the kernel image
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf
