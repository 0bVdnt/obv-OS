# RISC-V Process Scheduler and Exception Handler

This directory contains the implementation of process scheduling and exception handling mechanisms for the RISC-V operating system (obv-OS). As part of the learning-basics series, this project demonstrates how to implement a basic process scheduler and handle system exceptions in a minimal kernel environment.

---

## Overview

The code demonstrates:
- Process scheduling with round-robin implementation
- Exception handling for system calls and interrupts
- Process state management and transitions
- Kernel stack management during exceptions
- SBI (Supervisor Binary Interface) integration

---

## Project Structure

- **kernel.c**  
  Contains the main kernel implementation including process scheduling, exception handling, and SBI integration.

- **kernel.h**  
  Provides declarations for process scheduling structures, exception handling interfaces, and SBI call definitions.

- **common.c / common.h**  
  Implements shared utility functions and basic system interfaces.

- **kernel.ld**  
  The linker script that defines the memory layout of the kernel.

- **opensbi-riscv32-generic-fw_dynamic.bin**  
  The OpenSBI firmware binary for bootstrapping and handling SBI calls.

- **run.sh**  
  Shell script to build and run the kernel on QEMU.

- **kernel.elf & kernel.map**  
  The output executable and memory map generated during build.

---

## Key Components

### Process Scheduling

#### Process States
```c
#define PROC_UNUSED   0   // Process slot is free
#define PROC_RUNNABLE 1   // Process is ready to run
```

- Processes can be in one of two states:
  - `PROC_UNUSED`: Process slot is available
  - `PROC_RUNNABLE`: Process is ready for execution

#### Process Control Block
```c
struct process {
    int pid;                // Process identifier
    int state;             // Current process state
    vaddr_t sp;            // Stack pointer
    uint8_t stack[8192];   // Kernel stack
};
```

### Exception Handling

#### Trap Frame Structure
```c
struct trap_frame {
    uint32_t ra;    // Return address
    uint32_t gp;    // Global pointer
    uint32_t tp;    // Thread pointer
    // ... other registers ...
    uint32_t sp;    // Stack pointer
} __attribute__((packed));
```

- Stores complete CPU state during exceptions
- Ensures safe context preservation
- Enables proper exception recovery

### SBI Integration

#### SBI Call Structure
```c
struct ret_sbi {
    long err;  // Error code
    long val;  // Return value
};
```

- Provides interface to OpenSBI services
- Handles system-level operations
- Manages hardware interactions

---

## Running Instructions

### Prerequisites
- RISC-V GNU Toolchain
- QEMU RISC-V Emulator
- OpenSBI Firmware

### Building and Running
1. **Make the run script executable:**
   ```bash
   chmod +x run.sh
   ```

2. **Compile and launch the kernel:**
   ```bash
   ./run.sh
   ```

This will compile the project and launch the kernel in QEMU.

---

## Expected Output

When you run the kernel, you should see output similar to the following:

```
OpenSBI v1.2
   ____                    _____ ____ _____
  / __ \                  / ____|  _ \_   _|
 | |  | |_ __   ___ _ __ | (___ | |_) || |
 | |  | | '_ \ / _ \ '_ \ \___ \|  _ < | |
  ...
  Boot HART Base ISA        : rv32imafdcsu
  Boot HART ISA Extensions  : none
  ...

starting process A
Astarting process B
BABABABABABABABABABABABA
```

The output shows:
1. OpenSBI firmware initialization (abbreviated)
2. Process scheduling demonstration with multiple processes

Note: The actual output may vary slightly depending on your specific implementation and configuration.

---

## Technical Implementation

### Process Scheduling
- Round-robin scheduling algorithm
- Maximum of 8 processes supported
- Process state transitions
- Stack management for each process

### Exception Handling
- Complete register state preservation
- Safe stack switching during exceptions
- Exception type identification
- Proper exception recovery

### SBI Integration
- System call handling
- Hardware interaction
- Error handling and reporting

---

## Implementation Details

### Process Scheduling
1. Process state management
2. Round-robin scheduling implementation
3. Process context preservation
4. Stack management

### Exception Handling
1. Exception detection and classification
2. Register state preservation
3. Exception-specific handling
4. Safe return to user mode

### SBI Integration
1. System call interface
2. Hardware abstraction
3. Error handling
4. Return value management

---

## Future Enhancements

Potential improvements include:
- Priority-based scheduling
- Preemptive scheduling
- More sophisticated exception handling
- Enhanced SBI integration
- Process creation and termination
- Memory protection mechanisms