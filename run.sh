#!/bin/bash
set -xue
QEMU=qemu-system-riscv32
# ============================================================================
# NOTES:
# - Starts QEMU automatically after building, so you can test the kernel and user program in one step.
# - set -xue: enables debug output, stops on error, and treats unset variables as errors.
# - QEMU command line: -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel kernel.elf
# - -Wl,-Tuser.ld: Use user.ld as the linker script to control memory layout
# - -Wl,-Map=shell.map: Generate a map file showing memory layout
# - -o shell.elf: Output an ELF executable for the user program
# - --set-section-flags .bss=alloc,contents: Set flags for the BSS section
# - -O binary: Output in raw binary format for the user program
# - -Ibinary: Input format is binary for the user program
# - -Oelf32-littleriscv: Output format is RISC-V 32-bit ELF for the user program
# ============================================================================
# Build Script for RISC-V Bare Metal Application
# ============================================================================
# This script builds a two-stage system:
# 1. User application (shell) that runs in user mode
# 2. Kernel that contains the embedded user application
#
# The build process involves:
# - Compiling the user application
# - Converting it to a binary
# - Embedding it into the kernel
# ============================================================================

# Define the path to the llvm-objcopy tool
# llvm-objcopy is used to manipulate object files and convert between formats
# Update this path according to the location of llvm-objcopy in the system
OBJCOPY=/lib/llvm-19/bin/llvm-objcopy

# Define the path to the clang compiler
# clang is used as the C compiler for both user and kernel code
CC=/usr/bin/clang

# Compiler flags:
# -std=c11: Use C11 standard for modern C features
# -O2: Optimize for speed while maintaining reasonable compilation time
# -g3: Include maximum debug information for debugging
# -Wall -Wextra: Enable all standard warnings and extra warnings for code quality
# --target=riscv32: Target RISC-V 32-bit architecture
# -ffreestanding: Indicate this is a freestanding environment (no OS)
# -nostdlib: Don't link against standard C library
CFLAGS="-std=c11 -O2 -g3 -Wall -Wextra --target=riscv32 -ffreestanding -nostdlib"

# ============================================================================
# Build the user application (shell)
# ============================================================================
# Compile user.c, common.c, and shell.c:
# - -Wl,-Tuser.ld: Use user.ld as the linker script to control memory layout
# - -Wl,-Map=shell.map: Generate a map file showing memory layout
# - -o shell.elf: Output an ELF executable
$CC $CFLAGS -Wl,-Tuser.ld -Wl,-Map=shell.map -o shell.elf shell.c user.c common.c

# Convert shell.elf to a raw binary:
# - --set-section-flags .bss=alloc,contents: Set flags for the BSS section
# - -O binary: Output in raw binary format
$OBJCOPY --set-section-flags .bss=alloc,contents -O binary shell.elf shell.bin

# Convert shell.bin to an object file:
# - -Ibinary: Input format is binary
# - -Oelf32-littleriscv: Output format is RISC-V 32-bit ELF
$OBJCOPY -Ibinary -Oelf32-littleriscv shell.bin shell.bin.o

# ============================================================================
# Build the kernel
# ============================================================================
# Compile kernel.c, common.c, and link with shell.bin.o:
# - -Wl,-Tkernel.ld: Use kernel.ld as the linker script
# - -Wl,-Map=kernel.map: Generate a map file for the kernel
# - -o kernel.elf: Output the kernel as an ELF executable
$CC $CFLAGS -Wl,-Tkernel.ld -Wl,-Map=kernel.map -o kernel.elf \
    kernel.c common.c shell.bin.o

# Start QEMU
$QEMU -machine virt -bios default -nographic -serial mon:stdio --no-reboot \
    -kernel kernel.elf
