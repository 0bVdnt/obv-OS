# RISC-V Application Execution

This directory implements and provides examples for running user-space applications on the RISC-V operating system (obv-OS). Building upon the memory management concepts, this section demonstrates how to create, build, and integrate simple user programs with the kernel.

---

## Overview

This section covers:

1.  **Application Preparation:** Creating the first user application executable.
2.  **Memory Layout:** Defining the memory layout for user programs using a linker script.
3.  **Userland Library:** Creating a minimal userland library for application startup and basic functions.
4.  **Separate Build Process:** Building the user application independently from the kernel.
5.  **Kernel Integration:** Embedding the user application binary into the kernel image for execution.

## Core Concepts & Implementation Details

### User Application Memory Layout

User applications are placed in a separate memory region (typically starting at `0x1000000`) to ensure isolation from the kernel's address space. A custom linker script (`user.ld`) defines this base address, the section layout for user programs, and includes assertions to prevent the executable from exceeding a defined maximum size.

### Userland Runtime Library (`user.c`, `user.h`)

A minimal C library is provided for user-space programs.
*   **`start` function:** Serves as the application's entry point, responsible for setting up the user stack and calling the application's `main` function.
*   **`exit` function:** Handles program termination (initially an infinite loop).
*   **`putchar` function:** Declared as a placeholder for future console output.

### The Example User Application (`shell.c`)

The `shell.c` file contains a minimal user application, which, at this stage, simply contains an infinite loop in its `main` function. Its purpose is to demonstrate the build process and memory layout, not interactive functionality.

### Application Build Process (`run.sh`)

The user application is built separately from the kernel using the `run.sh` script. This process generates several important files:

1.  **Compilation & Linking:** The application source files (`shell.c`, `user.c`, `common.c`) are compiled and linked using the `user.ld` linker script, producing `shell.elf`.
    *   **`shell.elf`**: The ELF (Executable and Linkable Format) file for the user application. It contains the full program with section and symbol information.
    *   **`shell.map`**: The linker map file for the user application, detailing the memory layout, section addresses, and symbol locations. Useful for debugging and verifying correct memory placement.

2.  **Raw Binary Extraction:** `llvm-objcopy` converts `shell.elf` into `shell.bin`.
    *   **`shell.bin`**: The raw binary image of the user application, containing only the program's memory image, suitable for embedding or direct loading.

3.  **ELF Object File Creation:** `llvm-objcopy` then converts `shell.bin` into `shell.bin.o`.
    *   **`shell.bin.o`**: An ELF object file that wraps the raw binary. It defines special symbols (`_binary_shell_bin_start`, `_binary_shell_bin_end`, `_binary_shell_bin_size`) that allow the kernel to easily access the application's binary data and size during loading. This object file is intended to be linked into the kernel image in later stages.

**Note:** After running `run.sh`, there will be no direct output on the terminal from the user application. The current `shell.c` is a minimal infinite loop and is not yet integrated with the kernel for execution or console output. Console output will be implemented in subsequent stages.

### Kernel Integration

The `shell.bin.o` (the ELF object file containing the application's raw binary) is designed to be embedded directly into the kernel image. The kernel will then be responsible for:
*   Setting up the user application's virtual memory space.
*   Copying the application's binary data (accessed via `_binary_shell_bin_start` and `_binary_shell_bin_size`) into the allocated memory region.
*   Creating a process for the user application and scheduling it. When the user process is scheduled, the kernel will switch to the user's page table and jump to the application's entry point (`start`).

Separate linker scripts (`user.ld` and `kernel.ld`) are used to define the distinct memory layouts for user and kernel space.

## Project Structure

```
application-execution/
├── user.c          # Minimal userland runtime library
├── user.h          # Header for userland library
├── shell.c         # Example user application (simple infinite loop)
├── user.ld         # Linker script for user programs
├── run.sh          # Script to build application and kernel
├── kernel.c        # (Linked) Kernel implementation
├── common.c        # (Linked) Common utility functions
├── common.h        # (Linked) Common type definitions
├── kernel.ld       # (Linked) Linker script for kernel
├── kernel.h        # (Linked) Kernel definitions
├── kernel.elf      # (Generated) Kernel ELF executable
├── kernel.map      # (Generated) Kernel memory map
├── shell.elf       # (Generated) User application ELF executable
├── shell.map       # (Generated) User application memory map
├── shell.bin       # (Generated) User application raw binary
├── shell.bin.o     # (Generated) User application ELF object file
└── README.md       # This file
```

## Building and Verifying Artifacts

To build the user application and generate its artifacts, run the following commands:

1.  **Make the run script executable:**
    ```bash
    chmod +x run.sh
    ```

2.  **Build and launch (generates application artifacts):**
    ```bash
    ./run.sh
    ```
    *This command will generate `shell.elf`, `shell.map`, `shell.bin`, and `shell.bin.o`. There will be no output from the user application on the terminal at this stage.*

### Inspecting Generated Files

You can inspect the generated files to verify correct initialization and memory layout:

*   **`llvm-nm shell.bin.o`**: Lists the special symbols that the kernel will use to locate and load the user application binary.
    ```bash
    $ llvm-nm shell.bin.o
    00010250 D _binary_shell_bin_end
    00010250 A _binary_shell_bin_size
    00000000 D _binary_shell_bin_start
    ```
    **Inference:** `shell.bin.o` is in a format suitable for embedding in C code. The symbols `_binary_shell_bin_start`, `_binary_shell_bin_size`, and `_binary_shell_bin_end` provide the kernel with the necessary information to load and execute the application image.

*   **`llvm-objdump -d shell.elf`**: Disassembles the ELF file, allowing you to inspect the generated machine code and verify the placement of code sections and the entry point.
    ```bash
    $ llvm-objdump -d shell.elf

    shell.elf:      file format elf32-littleriscv

    Disassembly of section .text:

    01000000 <start>:
    1000000: 01010537      lui     a0, 0x1010
    1000004: 25050513      addi    a0, a0, 0x250
    1000008: 812a          mv      sp, a0
    100000a: 2011          jal     0x100000e <main>
    100000c: 2011          jal     0x1000010 <exit>

    0100000e <main>:
    100000e: a001          j       0x100000e <main>

    01000010 <exit>:
    1000010: a001          j       0x1000010 <exit>
    ```
    **Inference:** This disassembly confirms the presence of the `start`, `main`, and `exit` functions and their basic implementation (loops). The `start` function correctly sets up the stack and calls `main`, followed by `exit`.

## Debugging

Debugging at this stage primarily involves:
*   Verifying the build process defined in `run.sh`.
*   Checking `shell.map` to ensure sections are placed correctly in memory.
*   Using `llvm-nm` and `llvm-objdump` (as shown above) to inspect the ELF and object files for correct symbol placement and code generation.
*   Ensuring that `shell.bin` and `shell.bin.o` are correctly generated for later integration with the kernel.