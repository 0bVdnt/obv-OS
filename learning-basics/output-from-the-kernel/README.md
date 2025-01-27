# Implementing output on RISC-V kernel

This project is a simple RISC-V kernel simulation using QEMU and Clang. It demonstrates basic kernel functionalities such as memory initialization, simple O/P operations, and a custom implementation of the `printf` function.

## Table of Contents

- [Overview](#overview)
- [Setup](#setup)
- [Building the Kernel](#building-the-kernel)
- [Running the Kernel](#running-the-kernel)
- [Code Structure](#code-structure)
- [Key Components](#key-components)

## Overview

The project simulates a basic RISC-V kernel environment. It includes a simple bootloader, kernel main function, and basic I/O operations using the Supervisor Binary Interface (SBI). The kernel is compiled using Clang and executed in a QEMU virtual machine.

## Setup

Ensure you have the following tools installed:

- **QEMU**: A generic and open-source machine emulator and virtualizer.
- **Clang**: A compiler for the C language family.
- **LLVM tools**: (`llvm-nm`, `llvm-objdump`)
## Building the Kernel

To build the kernel, execute the `run.sh` script. This script compiles the kernel source files and links them into an ELF executable.
The script uses Clang with specific flags to target the RISC-V architecture and produce a freestanding binary without standard libraries.


```sh
  ./run.sh
```
## Running the Kernel

After building the kernel, the `run.sh` script will automatically start QEMU to run the kernel.

## Code Structure
- **`kernel.c`**: Contains the main kernel logic, including the `kernel_main` function and SBI call implementations.
- **`common.c`**: Implements utility functions like `printf`.
- **`kernel.ld`**: The linker script that defines the memory layout of the kernel.
- **`run.sh`**: A shell script to build and run the kernel using Clang and QEMU.

## Key Components

### Boot Process

The boot process is initiated by the `boot` function, which sets up the stack and jumps to `kernel_main`. This function is placed in a special section `.text.boot` to ensure it is located at the correct address.

### Memory Initialization

The `.bss` section is initialized to zero in `kernel_main` using a custom `memset` function. This ensures all uninitialized global variables are set to zero.

### O/P Operations

The kernel uses the SBI to perform O/P operations. The `putchar` function outputs characters, and a custom `printf` function formats and outputs strings and numbers.

### Linker Script

The `kernel.ld` script defines the memory layout, specifying where different sections like `.text`, `.rodata`, and `.bss` are placed in memory.

## Conclusion

This project provides a basic framework for understanding kernel development on the RISC-V architecture. It demonstrates essential concepts such as bootloading, memory management, and O/P operations in a minimal kernel environment.
