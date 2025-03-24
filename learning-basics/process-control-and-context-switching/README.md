# RISC-V Process Control and Context Switching

This directory contains a basic implementation of process control and context switching for the RISC-V operating system (obv-OS). As part of the learning-basics series, this project demonstrates how multi-tasking is implemented through efficient process management and CPU context switching in a minimal kernel environment.

---

## Overview

The code demonstrates:
- Process Control Block (PCB) implementation for process tracking
- Context switching mechanism between multiple processes
- Kernel stack setup and management for each process
- Register preservation during context switching
- Trap handling for interrupt and exception management

---

## Project Structure

- **kernel.c**  
  Contains the main kernel implementation including process creation, context switching, and trap handling functions. This file demonstrates the core process control functionality.

- **kernel.h**  
  Provides declarations for kernel functions, process control structures, and trap handling interfaces.

- **common.c / common.h**  
  Implements shared utility functions for memory operations, string handling, and output functions like printf.

- **kernel.ld**  
  The linker script that defines the memory layout of the kernel, including sections for code, data, BSS, and process stacks.

- **opensbi-riscv32-generic-fw_dynamic.bin**  
  The OpenSBI firmware binary necessary for bootstrapping and handling SBI calls in supervisor mode.

- **run.sh**  
  Shell script to build and run the kernel on a RISC-V emulator (QEMU).

- **kernel.elf & kernel.map**  
  The output executable and memory map generated during the build process.

---

## Key Functions and Concepts

### Process Control Block (`struct process`)
- **Purpose:**  
  Represents an individual process in the kernel.
- **Components:**
  - `pid`: Process identifier.
  - `state`: Current state of the process (UNUSED or RUNNABLE).
  - `sp`: Stack pointer for the process.
  - `stack`: 8KB kernel stack array for the process.

### Context Switching (`switch_context`)
- **Overview:**  
  Enables the kernel to switch execution between different processes - the core mechanism for multi-tasking.
- **Details:**
  - Implemented in assembly to directly manipulate CPU registers and stack pointers
  - Saves callee-saved registers (ra, s0-s11) of the current process onto its stack
  - Updates the stack pointers for both processes (storing current SP and loading next SP)
  - Restores callee-saved registers of the next process from its stack
  - Returns to continue execution in the context of the next process
  - Handles only callee-saved registers because caller-saved registers are already preserved by the calling convention
  - CPU resumes execution in the next process exactly where it left off previously

### Process Creation (`create_process`)
- **Role:**  
  Initializes a new process with its own execution context.
- **Key Steps:**
  - Finds an unused process slot in the process table.
  - Sets up the initial stack with zeroed registers and return address.
  - Initializes process fields like PID and state.
  - Returns a pointer to the new process control block.

### Delay Function (`delay`)
- **Purpose:**  
  Simulates computational work and slows down process execution for demonstration purposes.
- **Implementation:**
  - Executes a loop with 100 million iterations of no-operation (NOP) instructions.
  - Creates a visible pause between character outputs ('A' and 'B') from each process.
  - Prevents the terminal from becoming unresponsive by controlling the rate of output.
  - Demonstrates how processes can yield control and resume execution.
  - In a real-world OS, this would be replaced by actual computational work or sleep operations.

### Trap Handling (`kernel_entry` and `handle_trap`)
- **Purpose:**  
  Manages interrupts and exceptions in the kernel.
- **Implementation:**
  - Saves all register state when an exception occurs.
  - Uses a reliable kernel stack (via sscratch register) for handling traps.
  - Allows for safe handling of exceptions even when they occur in user space.

---

## Running Instructions

### Prerequisites
- **RISC-V GNU Toolchain or Clang:**  
  Ensure you have a working RISC-V compiler and associated utilities.
- **QEMU RISC-V Emulator:**  
  Used to emulate RISC-V hardware.
- **OpenSBI Firmware:**  
  Included in this project to provide SBI (Supervisor Binary Interface) support.

### Building and Running the Kernel
1. **Make the run script executable:**
   ```bash
   chmod +x run.sh
   ```

2. **Compile and launch the kernel:**
   ```bash
   ./run.sh
   ```

This process will generate the `kernel.elf` executable and `kernel.map` file, then boot the kernel. Once running, you should observe:
1. System initialization messages
2. "starting process A" and "starting process B" messages
3. Alternating 'A' and 'B' characters appearing on screen as the kernel switches between processes

## How It Works

1. The system initializes in `kernel_main()` and sets up two processes with their own PCBs
2. Each process has its own kernel stack and execution context stored in its Process Control Block
3. The `switch_context()` function performs the actual context switch between processes:
   - Saves all callee-saved registers of the current process onto its kernel stack
   - Switches the stack pointer to the next process's stack
   - Restores all callee-saved registers of the next process from its stack
   - Returns control to the next process at the point it was last suspended
4. The entire context switch is transparent to the running processes - they continue execution as if nothing happened
5. Each process runs in an infinite loop, voluntarily yielding control through context switching
6. The `delay()` function is called after each context switch to slow down execution, making the process switching visible to the user

### What to Expect During Execution
When the system runs, it demonstrates basic multi-tasking by alternating between two processes. Each process has its own execution context and prints its identifier ('A' or 'B') to show when it's active.

### Exiting QEMU
To exit the QEMU emulator:
- Press `Ctrl+A`, then `X` (press Ctrl and A together, release, then press X)

This key sequence sends a special command to QEMU to terminate the emulator.

---

## Additional Notes

- **Context Switching Implementation:**  
  The context switching mechanism in this project is a fundamental building block for multi-tasking operating systems. It preserves only the callee-saved registers as specified by the RISC-V calling convention, which is efficient yet sufficient for proper context preservation.
  
- **Trap Handling:**  
  The trap handling system is designed to safely save and restore processor state during exceptions, ensuring that interrupts don't corrupt process execution contexts.
  
- **Process Model:**  
  This implementation uses a very simple process model where each process runs in an infinite loop. In a more sophisticated system, processes could be scheduled based on priorities, time slices, or other criteria.

- **Artificial Delays:**  
  The `delay()` function is used as a simple mechanism to demonstrate process switching visually. It also serves a practical purpose by preventing the terminal from becoming unresponsive due to excessive output. In a production OS, this would be replaced by actual time-slicing mechanisms, scheduler algorithms, or process blocking on I/O or other resources.

### Future Enhancements

Potential improvements to this basic implementation could include:
- Adding a scheduler to manage multiple processes more dynamically
- Implementing process priority levels
- Supporting proper process termination and resource cleanup
- Adding inter-process communication mechanisms
- Implementing user-mode process execution with protected memory
- Replacing the artificial `delay()` with proper time-based scheduling

---
## Key Components for Understanding

The most important parts for understanding process control and context switching are:

- `struct process` in `kernel.h` - Defines the Process Control Block for tracking process state
- `switch_context()` in `kernel.c` - The core function that performs context switching between processes
- `create_process()` in `kernel.c` - Sets up a new process with its own stack and initial context
- `proc_a_entry()` and `proc_b_entry()` - The entry points for the two processes that demonstrate switching
- `delay()` in `kernel.c` - Simulates work to demonstrate process switching
- The trap handling code - Shows how to safely handle exceptions without corrupting process contexts