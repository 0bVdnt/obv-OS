#!/bin/bash
# Build and Run Script for RISC-V Kernel
# 
# This script handles:
# 1. Build Configuration
#    - Compiler setup (Clang)
#    - Compiler flags for RISC-V
#    - Linker configuration
# 
# 2. Kernel Building
#    - Compiles kernel.c and common.c
#    - Links with kernel.ld
#    - Generates map file for debugging
# 
# 3. QEMU Configuration
#    - RISC-V 32-bit machine
#    - VirtIO platform
#    - Serial console setup
# 
# 4. Debug Support
#    - Generates debug symbols
#    - Creates memory map
#    - Enables QEMU monitor

set -xue  # Enable strict error checking and command echoing

# QEMU configuration for RISC-V 32-bit
QEMU=qemu-system-riscv32

# Compiler configuration
CC=/usr/bin/clang
# Compiler flags:
# -std=c11: Use C11 standard
# -O2: Optimize for speed
# -g3: Include debug information
# -Wall -Wextra: Enable warnings
# --target=riscv32: Target RISC-V 32-bit
# -ffreestanding: No standard library
# -nostdlib: Don't link standard library
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# Build the kernel
# -Wl,-Tkernel.ld: Use kernel.ld as linker script
# -Wl,-Map=kernel.map: Generate memory map
# -o kernel.elf: Output ELF binary
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c common.c
# Note: -Wl, passes options to the linker instead of the C compiler.
# clang command does C compilation and executes the linker internally.

# Start QEMU with kernel
# -machine virt: Use VirtIO platform
# -bios default: Use default BIOS
# -nographic: No graphical output
# -serial mon:stdio: Use stdio for serial console and QEMU monitor
# --no-reboot: Don't reboot on kernel panic
# -kernel kernel.elf: Load kernel binary
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf
