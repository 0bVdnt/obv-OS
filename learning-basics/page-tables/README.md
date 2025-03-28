# RISC-V Page Tables Implementation

This directory contains the implementation of virtual memory management using page tables for the RISC-V operating system (obv-OS). As part of the learning-basics series, this project demonstrates how to implement a two-level page table structure (Sv32 mode) and manage virtual memory in a minimal kernel environment.

---

## Overview

The kernel implements:
1. Virtual Memory Management
   - Two-level page tables (Sv32 mode)
   - Page allocation and mapping
   - Memory protection and isolation

2. Process Management
   - Process creation and scheduling
   - Context switching
   - Process isolation through separate page tables

3. System Interface
   - SBI (Supervisor Binary Interface) for hardware operations
   - Console output
   - Trap handling
   
4. Boot Process
   - Memory initialization
   - Process setup
   - Trap vector configuration

## Theory

### Virtual Memory and Page Tables
When a program accesses memory, the CPU translates the specified address (virtual address) into a 
physical address. The table that maps virtual addresses to physical addresses is called a page 
table. By switching page tables, the same virtual address can point to different physical 
addresses, allowing:
- Isolation of memory spaces (virtual address spaces)
- Separation of kernel and application memory areas
- Enhanced system security

### RISC-V Sv32 Virtual Address Structure
The 32-bit virtual address is divided into:
- First-level page table index (VPN[1]): 10 bits
- Second-level index (VPN[0]): 10 bits
- Page offset: 12 bits

Example virtual address breakdown:
```
Virtual Address: 0x1000_0000
VPN[1] (10 bits): 0x040
VPN[0] (10 bits): 0x000
Offset (12 bits): 0x000
```

This structure utilizes the principle of locality, allowing for:
- Smaller page table sizes
- More effective use of the Translation Lookaside Buffer (TLB)

### Page Table Structure

The kernel uses a two-level page table structure:
1. First-level page table (table1)
   - Contains entries pointing to second-level tables
   - Indexed by VPN[1]
   - Maps 4MB regions (1024 * 4KB)

2. Second-level page table (table0)
   - Contains actual page mappings
   - Indexed by VPN[0]
   - Maps 4KB pages

## Project Structure

```
page-tables/
├── kernel.c      # Main kernel implementation
├── kernel.h      # Kernel definitions and structures
├── common.c      # Common utility functions
├── common.h      # Common type definitions
├── kernel.ld     # Linker script
├── run.sh        # Build and run script
└── README.md     # This file
```

## Key Components

### Virtual Memory Management

#### Page Table Flags
```c
#define SATP_SV32 (1u << 31)  // Enable Sv32 paging mode
#define PAGE_V    (1 << 0)    // Page table entry valid bit
#define PAGE_R    (1 << 1)    // Page is readable
#define PAGE_W    (1 << 2)    // Page is writable
#define PAGE_X    (1 << 3)    // Page is executable
#define PAGE_U    (1 << 4)    // Page is user-accessible
```

#### Process Control Block
```c
struct process {
    int pid;                    // Process identifier
    int state;                  // Current process state
    vaddr_t sp;                 // Stack pointer
    uint32_t *page_table;       // Process page table
    uint8_t stack[8192];        // Process kernel stack
};
```

### Page Table Structure

#### Two-Level Page Table
- First-level page table (table1)
  - Contains entries pointing to second-level tables
  - Indexed by VPN[1] (10 bits)
  - Maps 4MB regions

- Second-level page table (table0)
  - Contains actual page mappings
  - Indexed by VPN[0] (10 bits)
  - Maps 4KB pages

### Virtual Address Structure
```
32-bit Virtual Address:
[VPN[1] (10 bits)][VPN[0] (10 bits)][Offset (12 bits)]
```

Example:
```
Virtual Address: 0x1000_0000
VPN[1] (10 bits): 0x040
VPN[0] (10 bits): 0x000
Offset (12 bits): 0x000
```

---

## Building and Running
### Prerequisites
- RISC-V GNU Toolchain
- QEMU RISC-V Emulator
- OpenSBI Firmware
- Clang compiler

### Building and Running
1. **Make the run script executable:**
   ```bash
   chmod +x run.sh
   ```

2. **Compile and launch the kernel:**
   ```bash
   ./run.sh
   ```
   
The build script:
1. Compiles the kernel with appropriate flags
2. Links with the provided linker script
3. Generates debug symbols and memory map
4. Launches QEMU with the kernel

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
1. OpenSBI firmware initialization
2. Process scheduling with page table switching
3. Alternating process execution with memory isolation

### Verifying Paging Functionality

While the output appears similar to the previous program, the key difference is the implementation of virtual memory through page tables. You can verify this using QEMU's debug features:

1. **Enable QEMU Monitor**:
   ```bash
   # Press Ctrl+A followed by C to enter QEMU monitor
   ```

2. **Check Memory Mappings**:
   ```bash
   (qemu) info mem
   ```
   This will show the virtual-to-physical memory mappings, confirming that:
   - Each process has its own page table
   - Kernel space is identity mapped
   - User space is isolated

3. **Examine Page Tables**:
   ```bash
   (qemu) xp /1024x 0x80253000
   ```
   This shows the actual page table entries, demonstrating:
   - Two-level page table structure
   - Page permissions and flags
   - Virtual-to-physical mappings

4. **Monitor Page Faults**:
   ```bash
   (qemu) info registers
   ```
   Check `scause` and `stval` registers to see:
   - Page fault types
   - Faulting addresses
   - Translation lookaside buffer (TLB) behavior

5. **Process-Specific Memory**:
   ```bash
   (qemu) xp /32x <process_stack_address>
   ```
   This shows that each process has its own isolated memory space.

The key differences from the previous program are:
- Memory access is now mediated through page tables
- Each process has its own virtual address space
- Memory protection is enforced at the hardware level
- TLB caching improves performance
- Page faults can occur and are handled

---

## Implementation Details

### Page Table Management
1. Page table creation and initialization
2. Virtual to physical address mapping
3. Page permission management
4. TLB invalidation

### Process Isolation
1. Process-specific page tables
2. Kernel space identity mapping
3. User space isolation
4. Memory protection enforcement

### SBI Integration
1. System call interface
2. Hardware abstraction
3. Error handling
4. Return value management

---

## Debugging

### Common Paging Issues and Solutions

1. **Paging Mode Not Set**
   ```c
   // Incorrect - missing SATP_SV32
   "csrw satp, %[satp]\n"
   
   // Correct
   "csrw satp, %[satp]\n"  // with SATP_SV32 | page_table
   ```
   Debug: Use `info mem` in QEMU monitor to check if paging is enabled

2. **Using Physical Address Instead of Page Number**
   ```c
   // Incorrect - using physical address
   SATP_SV32 | ((uint32_t)next->page_table)
   
   // Correct - using page number
   SATP_SV32 | ((uint32_t)next->page_table / PAGE_SIZE)
   ```
   Debug: Check QEMU logs for page faults and examine register values

### Debugging Tools

1. **QEMU Monitor Commands**
   ```bash
   # View memory mappings
   (qemu) info mem
   
   # Dump registers
   (qemu) info registers
   
   # View page table contents
   (qemu) xp /1024x 0x80253000
   ```

2. **QEMU Logging**
   Add these flags to run.sh for detailed logging:
   ```bash
   -d unimp,guest_errors,int,cpu_reset -D qemu.log
   ```

3. **Memory Dumps**
   - Use `llvm-objdump` to examine the kernel binary
   - Check memory layout with `kernel.map`
   - Analyze page table entries with QEMU's memory examiner

### Common Debugging Scenarios

1. **Page Faults**
   - Check `scause` register for fault type
   - Examine `stval` for faulting address
   - Verify page table entries with `info mem`

2. **TLB Issues**
   - Ensure proper `sfence.vma` instructions
   - Verify page table updates
   - Check alignment requirements

3. **Memory Access Problems**
   - Verify page permissions
   - Check address translation
   - Examine memory mappings

## Future Enhancements

Potential improvements include:
- Demand paging
- Page swapping
- Shared memory regions
- Memory-mapped I/O
- Enhanced page protection
- Page fault handling
- Memory allocation strategies