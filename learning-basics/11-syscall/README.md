# RISC-V System Call (Syscall/ECALL) Support

This directory demonstrates the implementation of a basic system call (syscall/ECALL) interface for user-space applications running on the RISC-V operating system (obv-OS). Building on the user mode and process isolation features, this section introduces a controlled mechanism for user programs to request services from the kernel, enabling safe user/kernel communication.

---

## Overview

This section covers:

1. **System Call Mechanism:** How user programs invoke kernel services using the ECALL instruction.
2. **Trap Handling:** How the kernel distinguishes and handles system calls from other exceptions.
3. **User/Kernel Interface:** The conventions for passing arguments and return values between user and kernel space.
4. **Extending User Programs:** How user applications use syscalls for output and other services.

## Key Concepts & Implementation Details

### What is a System Call?
A system call (syscall) is a controlled entry point for user programs to request privileged operations from the kernel. In RISC-V, this is typically done using the `ecall` instruction, which triggers a trap to the kernel.

### New Features in This Section
- **Syscall Numbering:** Each system call is assigned a unique number (e.g., `SYS_PUTCHAR`).
- **User-Space Syscall Wrapper:** User programs call a `syscall` function, which sets up arguments and issues an `ecall`.
- **Kernel Trap Handler:** The kernel's trap handler recognizes ECALLs from user mode, extracts syscall numbers and arguments, and dispatches to the appropriate kernel service.
- **Return Values:** The kernel places the result of the syscall in a register, which is returned to the user program.
- **Example Syscall:** `putchar` is implemented as a syscall, allowing user programs to print characters via the kernel.

### System Call Flow
1. **User Program:** Calls a wrapper (e.g., `putchar`), which invokes the generic `syscall` function.
2. **ECALL Instruction:** The `syscall` function sets up registers and executes `ecall`.
3. **Trap to Kernel:** The CPU switches to supervisor mode, and the kernel's trap handler is invoked.
4. **Syscall Dispatch:** The kernel identifies the syscall number and arguments, performs the requested service, and sets the return value.
5. **Return to User:** The kernel resumes user mode, returning the result to the user program.

### Files and Structure

```
11-syscall/
├── user.c          # User mode runtime library (syscall wrappers)
├── user.h          # User mode syscall definitions
├── shell.c         # Example user application using syscalls
├── user.ld         # Linker script for user programs
├── run.sh          # Build and test script
├── kernel.c        # Kernel with syscall/trap handling
├── kernel.h        # Kernel definitions (trap frame, syscall numbers)
├── common.c        # Common utility functions
├── common.h        # Common type definitions and syscall numbers
├── kernel.ld       # Kernel linker script
├── kernel.elf      # Generated kernel executable
├── kernel.map      # Kernel memory map
├── shell.elf       # User application ELF
├── shell.map       # User application memory map
├── shell.bin       # User application binary
├── shell.bin.o     # User application object file
└── README.md       # This file
```

## Example: User Output via Syscall

In `shell.c`, user output is now performed via a syscall:
```c
printf("A"); // Triggers a syscall to the kernel to print 'A'
```
- The user program does **not** access hardware or kernel memory directly.
- The kernel validates and performs the output, maintaining isolation and security.

## Trap and Syscall Handling in the Kernel
- The kernel's trap handler (`handle_trap`) checks if the trap is an ECALL from user mode.
- If so, it calls `handle_syscall`, which dispatches based on the syscall number (e.g., `SYS_PUTCHAR`).
- Arguments are passed via registers (e.g., `a0`, `a1`), and the return value is placed in `a0`.
- The kernel then resumes user mode execution.

## Extending the Syscall Interface
- New syscalls can be added by defining new syscall numbers and implementing their handlers in the kernel.
- User programs can use these syscalls by adding wrappers in `user.c`/`user.h`.

## Building and Testing

To build and test the system call support:

1. **Make the run script executable (if using a Unix-like shell):**
    ```bash
    chmod +x run.sh         
    ```
2. **Build and launch:**
    ```bash
    ./run.sh
    ```
    This will:
    * Build the kernel and user application
    * Start QEMU automatically
    * Run the user program, demonstrating system call-based output
3. **Expected Output:**
    ```bash
    Hello from the shell!
    ```
## Learning More
- Compare with the previous section (`10-user-mode`) to see how user/kernel separation and communication are improved.

## Summary
This section introduces a minimal, real-world system call interface for RISC-V, providing a foundation for safe, extensible user/kernel interaction. All user/kernel communication now happens through well-defined syscalls, improving security, modularity, and OS design clarity.
