# RISC-V User Mode Support

This directory implements and provides examples for running user-space applications in user mode on the RISC-V operating system (obv-OS). Building upon the application execution concepts, this section demonstrates how to create, load, and execute user programs in a protected user mode environment.

---

## Overview

This section covers:

1. **User Mode Transition:** Implementing the switch from supervisor mode to user mode.
2. **Process Creation:** Creating user processes with proper memory isolation.
3. **Memory Protection:** Setting up page tables with user mode permissions.
4. **Trap Handling:** Handling user mode exceptions and faults.
5. **Process Scheduling:** Managing user processes alongside kernel processes.

## Core Concepts & Implementation Details

### User Mode Memory Layout

User applications are placed in a separate memory region starting at `USER_BASE` (0x1000000) with proper user mode permissions. The page tables are configured to:
* Map kernel space with supervisor-only access
* Map user space with user-accessible permissions (PAGE_U)
* Maintain memory isolation between processes

### User Mode Transition (`user_entry`)

The transition to user mode is implemented through:
* Setting up the user program's entry point in `sepc`
* Configuring `sstatus` with appropriate flags (SSTATUS_SPIE)
* Using the `sret` instruction to switch to user mode

### Process Creation and Management

The kernel now supports:
* Creating user processes with their own page tables
* Copying user program images into user space
* Setting up proper memory protection
* Managing process state and scheduling

### Trap Handling

The trap handler has been enhanced to:
* Handle user mode exceptions (page faults, illegal instructions)
* Provide information about the cause of traps
* Maintain system stability when user programs misbehave

### The Example User Application (`shell.c`)

The `shell.c` file demonstrates user mode isolation by:
* Attempting to access kernel memory (which triggers a page fault)
* Running in an infinite loop
* Operating in user mode with restricted privileges

### Build Process (`run.sh`)

The build process remains similar but now includes:
* Automatic QEMU launch for testing
* Proper linking of user programs with user mode support
* Generation of all necessary binaries and maps

## Project Structure

```
user-mode/
├── user.c          # User mode runtime library
├── user.h          # User mode header definitions
├── shell.c         # Example user application
├── user.ld         # Linker script for user programs
├── run.sh          # Build and test script
├── kernel.c        # Kernel with user mode support
├── kernel.h        # Kernel definitions
├── common.c        # Common utility functions
├── common.h        # Common type definitions
├── kernel.ld       # Kernel linker script
├── kernel.elf      # Generated kernel executable
├── kernel.map      # Kernel memory map
├── shell.elf       # User application ELF
├── shell.map       # User application memory map
├── shell.bin       # User application binary
├── shell.bin.o     # User application object file
└── README.md       # This file
```

## Building and Testing

To build and test the user mode support:

1. **Make the run script executable:**
    ```bash
    chmod +x run.sh
    ```

2. **Build and launch:**
    ```bash
    ./run.sh
    ```
    Expected output:
    ```bash
    PANIC: kernel.c:272: unexpected trap scause=0000000f, stval=80500000, sepc=0100001a
    ```
    This will:
    * Build the kernel and user application
    * Start QEMU automatically
    * Run the user program in user mode
    * Demonstrate memory protection through a page fault

### Verifying User Mode Operation

You can verify the user mode operation through:

1. **QEMU Monitor Commands:**
    ```bash
    # Check the current mapping of execution image
    (qemu) info mem
    # Check the CPU state including the program counter in user space
    (qemu) info registers
    # Examine memory contents at a specific address (replace XXXXXXX with actual address)
    (qemu) xp /32b 0xXXXXXXX
    # Disassemble machine code at a specific address
    (qemu) xp /8i 0xXXXXXXX
    ```

2. **Debugging Tools:**
    ```bash
    # Check the contents of shell.bin
    $ hexdump -C shell.bin | head
    # Disassemble and check the contents of shell.elf
    $ llvm-objdump -d shell.elf | head -n20
    # Check the source line corresponding to the fault address
    $ llvm-addr2line -e shell.elf 0x100001a
    ```

3. **Trap Information:**
    When the user program attempts to access kernel memory, you'll see a trap message:
    ```
    PANIC: kernel.c:272: unexpected trap scause=0000000f, stval=80500000, sepc=0100001a
    ```
    This indicates:
    * A page fault (scause=0xf) in user mode
    * The fault occurred at virtual address 0x80500000
    * The program counter was at 0x100001a when the fault occurred

## Debugging

Debugging user mode support involves:
* Checking page table configurations using QEMU's `info mem` command
* Verifying user mode transitions by examining CPU state with `info registers`
* Monitoring trap handling through the panic messages
* Inspecting process state and scheduling
* Using QEMU's monitor commands to examine memory and code

Common issues to watch for:
* Incorrect page table permissions (check with `info mem`)
* Misaligned memory accesses (verify with `xp` commands)
* Improper user mode transition setup (check `sstatus` and `sepc` in `info registers`)
* Process scheduling problems (monitor process state transitions)